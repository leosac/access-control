/*
    Copyright (C) 2014-2016 Islog

    This file is part of Leosac.

    Leosac is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Facade.hpp"
#include "ExceptionConverter.hpp"
#include "core/CoreUtils.hpp"
#include "core/UserSecurityContext.hpp"
#include "core/audit/IWSAPICall.hpp"
#include "odb/session.hxx"
#include "tools/AssertCast.hpp"
#include "tools/Colorize.hpp"
#include "tools/db/DBService.hpp"
#include "tools/log.hpp"
#include <exception>
#include <map>
#include <zmqpp/zmqpp.hpp>

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{

Facade::Facade(zmqpp::reactor &reactor, CoreUtilsPtr utils)
    : reactor_(reactor)
    , utils_(utils)
    , dealer_(utils->zmqpp_context(), zmqpp::socket_type::dealer)
{
    dealer_.connect("inproc://SERVICE.WEBSOCKET");
    reactor_.add(dealer_, std::bind(&Facade::handle_dealer, this));
}


Facade::Facade(zmqpp::reactor &reactor, CoreUtilsPtr utils,
               const std::string module_name)
    : Facade(reactor, utils)
{
    zmqpp::message msg;
    msg << "REGISTER_MODULE" << module_name;
    bool sent = dealer_.send(msg, true);
    ASSERT_LOG(sent, "Internal send would have blocked.");
}

void Facade::send_message(const std::string &connection_identifier,
                          const std::string &content)
{
    zmqpp::message msg;

    msg << "SEND_MESSAGE" << connection_identifier << content;
    bool sent = dealer_.send(msg, true);
    ASSERT_LOG(sent, "Internal send would have blocked.");
}

void Facade::send_message(const std::string &connection_identifier,
                          const ServerMessage &msg)
{
    json json_message;

    json_message["uuid"]          = msg.uuid;
    json_message["type"]          = msg.type;
    json_message["status_code"]   = static_cast<int64_t>(msg.status_code);
    json_message["status_string"] = msg.status_string;
    json_message["content"]       = msg.content;

    send_message(connection_identifier, json_message.dump(4));
}

void Facade::handle_dealer()
{
    zmqpp::message msg;
    dealer_.receive(msg);

    if (msg.parts() == 1)
    {
        // Probably a response to REGISTER_HANDLER. Should work, otherwise
        // we abort.
        std::string tmp;
        msg >> tmp;
        ASSERT_LOG(tmp == "OK", "Something failed.");
        return;
    }

    // A websocket message has arrived.
    ASSERT_LOG(msg.parts() == 3, "Ill formed message.");
    ModuleIncomingMessage iws_msg;
    msg >> iws_msg.audit_id >> iws_msg.connection_identifier >> iws_msg.content;

    json ws_request = json::parse(iws_msg.content);
    WebSockAPI::ClientMessage client_message;

    // Extract general message argument. This is sent by the WebSockAPI module
    // so we assume it is correct.
    client_message.uuid    = ws_request.at("uuid");
    client_message.type    = ws_request.at("type");
    client_message.content = ws_request.at("content");

    odb::session odb_session;
    WebSockAPI::ModuleRequestContext ctx;
    ctx.dbsrv = std::make_shared<DBService>(utils_->database());

    auto wsapi_call_audit =
        assert_cast<Audit::IWSAPICallPtr>(ctx.dbsrv->find_audit_by_id(
            iws_msg.audit_id, DBService::THROW_IF_NOT_FOUND));
    ctx.audit = wsapi_call_audit;

    UserSecurityContext security_ctx(ctx.dbsrv, wsapi_call_audit->author_id());
    ctx.security_ctx = &security_ctx;

    WebSockAPI::ServerMessage ret = dispatch(ctx, client_message);
    {
        odb::transaction t(utils_->database()->begin());
        wsapi_call_audit->status_code(ret.status_code);
        wsapi_call_audit->status_code(ret.status_code);
        wsapi_call_audit->status_string(ret.status_string);
        wsapi_call_audit->response_content(ret.content.dump(4));
        wsapi_call_audit->finalize();
        t.commit();
    }

    send_message(iws_msg.connection_identifier, ret);
}

void Facade::send_handler_registration_message(const std::string &type)
{
    zmqpp::message msg;
    msg << "REGISTER_HANDLER";
    msg << type;
    bool sent = dealer_.send(msg, true);
    ASSERT_LOG(sent, "Internal send would have blocked.");
}

ServerMessage Facade::dispatch(const ModuleRequestContext &mrc,
                               const ClientMessage &msg)
{
    auto handler = handlers_.find(msg.type);
    ASSERT_LOG(handler != handlers_.end(), "No handler for "
                                               << Colorize::cyan(msg.type));
    return handler->second(mrc, msg);
}

void Facade::register_crud_handler(
    const std::string &type,
    ExternalCRUDResourceHandler::Factory crud_handler_factory)
{
    auto wrapper = [=](const ModuleRequestContext &mrc,
                       const ClientMessage &msg) -> WebSockAPI::ServerMessage {

        WebSockAPI::ServerMessage response;
        response.status_code = APIStatusCode::SUCCESS;
        response.content     = {};
        try
        {
            response.uuid = msg.uuid;
            response.type = msg.type;

            auto opt_json = crud_handler_factory(mrc)->process(msg);
            if (opt_json)
            {
                response.content = *opt_json;
            }
        }
        catch (...)
        {
            return WebSockAPI::ExceptionConverter().convert_merge(
                std::current_exception(), response);
        }
        return response;
    };
    handlers_[type + ".read"] = wrapper;
    send_handler_registration_message(type + ".read");
    handlers_[type + ".update"] = wrapper;
    send_handler_registration_message(type + ".update");
    handlers_[type + ".create"] = wrapper;
    send_handler_registration_message(type + ".create");
    handlers_[type + ".delete"] = wrapper;
    send_handler_registration_message(type + ".delete");
}

void Facade::register_handler_simple(const std::string &type,
                                     SimpleHandlerT callable, ActionActionParam perm)
{
    auto wrapper = [=](const ModuleRequestContext &mrc,
                       const ClientMessage &msg) -> WebSockAPI::ServerMessage {
        WebSockAPI::ServerMessage response;
        response.status_code = APIStatusCode::SUCCESS;
        response.content     = {};
        try
        {
            response.uuid = msg.uuid;
            response.type = msg.type;

            mrc.security_ctx->enforce_permission(perm.first, perm.second);
            response.content = callable(mrc, msg);
        }
        catch (...)
        {
            return WebSockAPI::ExceptionConverter().convert_merge(
                std::current_exception(), response);
        }
        return response;
    };
    register_handler(type, wrapper);
}
}
}
}
