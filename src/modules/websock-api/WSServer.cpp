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

#include "WSServer.hpp"
#include "Exceptions.hpp"
#include "Token_odb.h"
#include "api/GroupCRUD.hpp"
#include "api/LogGet.hpp"
#include "api/MembershipGet.hpp"
#include "api/PasswordChange.hpp"
#include "api/UserCRUD.hpp"
#include "core/audit/AuditFactory.hpp"
#include "core/audit/WSAPICall.hpp"
#include "core/auth/User.hpp"
#include "exception/ExceptionsTools.hpp"
#include "exception/ModelException.hpp"
#include "tools/db/DBService.hpp"
#include "tools/db/DatabaseTracer.hpp"
#include "tools/db/MultiplexedTransaction.hpp"
#include "tools/log.hpp"
#include <json.hpp>
#include <odb/session.hxx>

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

using json = nlohmann::json;

WSServer::WSServer(WebSockAPIModule &module, DBPtr database)
    : auth_(*this)
    , dbsrv_(std::make_shared<DBService>(database))
    , module_(module)
{
    ASSERT_LOG(database, "No database object passed into WSServer.");
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;
    srv_.init_asio();

    srv_.set_open_handler(std::bind(&WSServer::on_open, this, _1));
    srv_.set_close_handler(std::bind(&WSServer::on_close, this, _1));
    srv_.set_message_handler(std::bind(&WSServer::on_message, this, _1, _2));
    srv_.set_reuse_addr(true);
    // clear all logs.
    // srv_.clear_access_channels(websocketpp::log::alevel::all);

    handlers_["get_leosac_version"]      = &APISession::get_leosac_version;
    handlers_["create_auth_token"]       = &APISession::create_auth_token;
    handlers_["authenticate_with_token"] = &APISession::authenticate_with_token;
    handlers_["logout"]                  = &APISession::logout;
    handlers_["system_overview"]         = &APISession::system_overview;

    handlers2_["get_logs"]        = &LogGet::create;
    handlers2_["membership_get"]  = &MembershipGet::create;
    handlers2_["password_change"] = &PasswordChange::create;

    register_crud_handler("group", &WebSockAPI::GroupCRUD::instanciate);
    register_crud_handler("user", &WebSockAPI::UserCRUD::instanciate);
}

void WSServer::on_open(websocketpp::connection_hdl hdl)
{
    INFO("New WebSocket connection !");
    connection_api_.insert(std::make_pair(hdl, std::make_shared<APISession>(*this)));
}

void WSServer::on_close(websocketpp::connection_hdl hdl)
{
    INFO("WebSocket connection closed.");
    connection_api_.erase(hdl);
}

void WSServer::on_message(websocketpp::connection_hdl hdl, Server::message_ptr msg)
{
    ASSERT_LOG(connection_api_.find(hdl) != connection_api_.end(),
               "Cannot retrieve API pointer from connection handle.");
    auto api_handle = connection_api_.find(hdl)->second;

    // todo maybe parse first so be we can have better error handling.
    auto global_request_count = dbsrv_->operation_count();
    Audit::IWSAPICallPtr audit;
    ServerMessage response;
    json req;
    try
    {
        audit = Audit::Factory::WSAPICall(dbsrv_->db());
    }
    catch (const odb::exception &e)
    {
        WARN("Database Error in WServer::on_message. Aborting request processing. "
             "Error: "
             << e.what());
        response.status_code   = APIStatusCode::DATABASE_ERROR;
        response.status_string = e.what();
        send_message(hdl, response);
        return;
    }
    try
    {
        audit->event_mask(Audit::EventType::WSAPI_CALL);
        audit->author(api_handle->current_user());
        auto ws_connection_ptr = srv_.get_con_from_hdl(hdl);
        ASSERT_LOG(ws_connection_ptr, "No websocket connection object from handle.");
        audit->source_endpoint(ws_connection_ptr->get_remote_endpoint());

        // todo careful potential DDOS as we store the full content without checking
        // for now.
        audit->request_content(msg->get_payload());
        req = json::parse(msg->get_payload());

        response = handle_request(api_handle, req, audit);
        INFO("Incoming payload: \n" << req.dump(4));
    }
    catch (const std::invalid_argument &e)
    {
        // JSON parse error
        response.status_code   = APIStatusCode::MALFORMED;
        response.status_string = "Failed to parse JSON.";
    }

    try
    {
        db::MultiplexedTransaction t(dbsrv_->db()->begin());
        // If something went wrong while processing the request, the audit object
        // may need to be reload. We might as well reload it everytime.
        audit->reload();
        // Update audit value.
        audit->uuid(response.uuid);
        audit->method(response.type);
        audit->status_code(response.status_code);
        audit->status_string(response.status_string);
        audit->response_content(response.content.dump(4));
        audit->database_operations(
            static_cast<uint16_t>(dbsrv_->operation_count() - global_request_count));
        audit->finalize();
        t.commit();
    }
    catch (const odb::exception &e)
    {
        WARN("Database Error in WServer::on_message. Failed to persist final audit: "
             << e.what());
        response.status_code   = APIStatusCode::DATABASE_ERROR;
        response.status_string = e.what();
        send_message(hdl, response);
        return;
    }

    send_message(hdl, response);
}

void WSServer::run(const std::string &interface, uint16_t port)
{
    boost::asio::ip::tcp::endpoint endpoint(
        boost::asio::ip::address::from_string(interface), port);
    srv_.listen(endpoint);
    srv_.start_accept();
    srv_.run();
}

void WSServer::start_shutdown()
{
    srv_.stop_listening();
    for (auto con_api : connection_api_)
    {
        srv_.close(con_api.first, 0, "bye");
    }
}

APIAuth &WSServer::auth()
{
    return auth_;
}

json WSServer::dispatch_request(APIPtr api_handle, const ClientMessage &in,
                                Audit::IAuditEntryPtr audit)
{
    // A request is an "Unit-of-Work" for the application.
    // We create a default database session for the request.
    odb::session database_session;
    auto handler_factory = handlers2_.find(in.type);
    api_handle->hook_before_request();

    if (handler_factory != handlers2_.end())
    {
        RequestContext ctx{
            .session = api_handle, .dbsrv = dbsrv_, .server = *this, .audit = audit};

        MethodHandlerUPtr method_handler = handler_factory->second(ctx);
        return method_handler->process(in);
    }

    auto handler_method = handlers_.find(in.type);
    if (handler_method != handlers_.end())
    {
        if (api_handle->allowed(in.type))
        {
            auto method_ptr = handler_method->second;
            return ((*api_handle).*method_ptr)(in.content);
        }
        else
        {
            throw PermissionDenied();
        }
    }

    auto crud_handler_factory = crud_handlers_.find(in.type);
    if (crud_handler_factory == crud_handlers_.end())
        throw InvalidCall();
    else
    {
        RequestContext ctx{
            .session = api_handle, .dbsrv = dbsrv_, .server = *this, .audit = audit};

        CRUDResourceHandlerUPtr crud_handler = crud_handler_factory->second(ctx);
        return crud_handler->process(in);
    }
}

DBPtr WSServer::db()
{
    return dbsrv_->db();
}

CoreUtilsPtr WSServer::core_utils()
{
    return module_.core_utils();
}

void WSServer::send_message(websocketpp::connection_hdl hdl,
                            const ServerMessage &msg)
{
    json json_message;

    json_message["uuid"]          = msg.uuid;
    json_message["type"]          = msg.type;
    json_message["status_code"]   = static_cast<int64_t>(msg.status_code);
    json_message["status_string"] = msg.status_string;
    json_message["content"]       = msg.content;

    srv_.send(hdl, json_message.dump(4), websocketpp::frame::opcode::text);
}

ClientMessage WSServer::parse_request(const json &req)
{
    ClientMessage msg;

    try
    {
        // Extract general message argument.
        msg.uuid    = req.at("uuid");
        msg.type    = req.at("type");
        msg.content = req.at("content");
    }
    catch (const std::out_of_range &e)
    {
        throw MalformedMessage();
    }
    catch (const std::domain_error &e)
    {
        throw MalformedMessage();
    }
    return msg;
}

ServerMessage WSServer::handle_request(APIPtr api_handle, const json &req,
                                       Audit::IAuditEntryPtr audit)
{
    ServerMessage response;
    response.status_code = APIStatusCode::SUCCESS;
    response.content     = {};
    try
    {
        ClientMessage input = parse_request(req);
        response.uuid       = input.uuid;
        response.type       = input.type;
        response.content    = dispatch_request(api_handle, input, audit);
    }
    catch (const InvalidCall &e)
    {
        response.status_code   = APIStatusCode::INVALID_CALL;
        response.status_string = e.what();
    }
    catch (const PermissionDenied &e)
    {
        response.status_code   = APIStatusCode::PERMISSION_DENIED;
        response.status_string = e.what();
    }
    catch (const MalformedMessage &e)
    {
        WARN("ST: " << e.trace().str());
        response.status_code   = APIStatusCode::MALFORMED;
        response.status_string = e.what();
    }
    catch (const SessionAborted &e)
    {
        response.status_code   = APIStatusCode::SESSION_ABORTED;
        response.status_string = e.what();
    }
    catch (const EntityNotFound &e)
    {
        response.status_code            = APIStatusCode::ENTITY_NOT_FOUND;
        response.status_string          = e.what();
        response.content["entity_id"]   = e.entity_id();
        response.content["entity_type"] = e.entity_type();
    }
    catch (const ModelException &e)
    {
        response.status_code       = APIStatusCode::MODEL_EXCEPTION;
        response.status_string     = e.what();
        response.content["errors"] = e.json_errors();
    }
    catch (const LEOSACException &e)
    {
        WARN("Leosac specific exception has been caught: " << e.what() << std::endl
                                                           << e.trace().str());
        response.status_code   = APIStatusCode::GENERAL_FAILURE;
        response.status_string = e.what(); // todo Maybe remove in production.
    }
    catch (const odb::exception &e)
    {
        ERROR("Database Error: " << e.what());
        response.status_code   = APIStatusCode::DATABASE_ERROR;
        response.status_string = "Database Error: " + std::string(e.what());
    }
    catch (const std::exception &e)
    {
        WARN("Exception when processing request: " << e.what());
        response.status_code   = APIStatusCode::GENERAL_FAILURE;
        response.status_string = e.what();
    }
    return response;
}

void WSServer::clear_user_sessions(Auth::UserPtr user, APIPtr exception,
                                   bool new_transaction /* = true */)
{
    ASSERT_LOG(
        new_transaction || odb::transaction::has_current(),
        "Not requesting a new transaction, but no currently active transaction.");

    std::vector<Auth::TokenPtr> tokens_to_remove;
    for (const auto &connection_to_api : connection_api_)
    {
        const auto &session = connection_to_api.second;
        if (session->current_user_id() == user->id() && exception != session)
        {
            // Mark the token for invalidation.
            if (auto token = session->current_token())
                tokens_to_remove.push_back(token);

            // Clear authentication status from this user.
            session->abort_session();
            // Notify them
            ServerMessage msg;
            msg.content["reason"] = "Session cleared.";
            msg.status_code       = APIStatusCode::SUCCESS;
            msg.type              = "session_closed";
            send_message(connection_to_api.first, msg);
        }
    }

    std::unique_ptr<db::MultiplexedTransaction> transaction;
    if (new_transaction)
        transaction =
            std::make_unique<db::MultiplexedTransaction>(dbsrv_->db()->begin());
    for (const auto &token : tokens_to_remove)
    {
        dbsrv_->db()->erase<Auth::Token>(token->id());
    }
    if (new_transaction)
        transaction->commit();
}

void WSServer::register_crud_handler(const std::string &resource_name,
                                     CRUDResourceHandler::Factory factory)
{
    crud_handlers_[resource_name + ".read"]   = factory;
    crud_handlers_[resource_name + ".update"] = factory;
    crud_handlers_[resource_name + ".create"] = factory;
    crud_handlers_[resource_name + ".delete"] = factory;
}

DBServicePtr WSServer::dbsrv()
{
    return dbsrv_;
}
