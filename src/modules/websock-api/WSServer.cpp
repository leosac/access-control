/*
    Copyright (C) 2014-2016 Leosac

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
#include "ExceptionConverter.hpp"
#include "Exceptions.hpp"
#include "Service.hpp"
#include "Token_odb.h"
#include "WebSockAPI.hpp"
#include "api/AccessOverview.hpp"
#include "api/AccessPointCRUD.hpp"
#include "api/AuditGet.hpp"
#include "api/CredentialCRUD.hpp"
#include "api/DoorCRUD.hpp"
#include "api/GroupCRUD.hpp"
#include "api/LogGet.hpp"
#include "api/MembershipCRUD.hpp"
#include "api/PasswordChange.hpp"
#include "api/ScheduleCRUD.hpp"
#include "api/UserCRUD.hpp"
#include "api/search/AccessPointSearch.hpp"
#include "api/search/DoorSearch.hpp"
#include "api/search/GroupSearch.hpp"
#include "api/search/ScheduleSearch.hpp"
#include "core/CoreUtils.hpp"
#include "core/GetServiceRegistry.hpp"
#include "core/audit/AuditFactory.hpp"
#include "core/audit/WSAPICall.hpp"
#include "core/auth/User.hpp"
#include "exception/EntityNotFound.hpp"
#include "exception/ExceptionsTools.hpp"
#include "exception/ModelException.hpp"
#include "exception/PermissionDenied.hpp"
#include "tools/db/DBService.hpp"
#include "tools/db/DatabaseTracer.hpp"
#include "tools/db/MultiplexedTransaction.hpp"
#include "tools/log.hpp"
#include "tools/registry/ThreadLocalRegistry.hpp"
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


    // Register internal handlers, ie handler that are managed by the Websocket
    // module itself.

    handlers_["get_leosac_version"]      = &APISession::get_leosac_version;
    handlers_["create_auth_token"]       = &APISession::create_auth_token;
    handlers_["authenticate_with_token"] = &APISession::authenticate_with_token;
    handlers_["logout"]                  = &APISession::logout;
    handlers_["system_overview"]         = &APISession::system_overview;

    individual_handlers_["audit.get"]                 = &AuditGet::create;
    individual_handlers_["get_logs"]                  = &LogGet::create;
    individual_handlers_["password_change"]           = &PasswordChange::create;
    individual_handlers_["search.group_name"]         = &GroupSearch::create;
    individual_handlers_["search.door_alias"]         = &DoorSearch::create;
    individual_handlers_["search.access_point_alias"] = &AccessPointSearch::create;
    individual_handlers_["search.schedule_name"]      = &ScheduleSearch::create;
    individual_handlers_["access_overview"]           = &AccessOverview::create;

    register_crud_handler("group", &WebSockAPI::GroupCRUD::instanciate);
    register_crud_handler("user", &WebSockAPI::UserCRUD::instanciate);
    register_crud_handler("user-group-membership",
                          &WebSockAPI::MembershipCRUD::instanciate);
    register_crud_handler("credential", &WebSockAPI::CredentialCRUD::instanciate);
    register_crud_handler("schedule", &WebSockAPI::ScheduleCRUD::instanciate);
    register_crud_handler("door", &WebSockAPI::DoorCRUD::instanciate);
    register_crud_handler("access_point", &WebSockAPI::AccessPointCRUD::instanciate);

    // Setup PUSH socket to talks to the parent module.
    to_module_ = std::make_unique<zmqpp::socket>(
        module.core_utils()->zmqpp_context(), zmqpp::socket_type::push);
    to_module_->connect("inproc://MODULE.WEBSOCKET.INTERNAL_PULL");
}

WSServer::~WSServer()
{
    ASSERT_LOG(get_service_registry().use_count<Service>() <= 0,
               "Someone is still using the WSService");
}

void WSServer::on_open(websocketpp::connection_hdl hdl)
{
    INFO("New WebSocket connection !");
    connection_session_.insert(
        std::make_pair(hdl, std::make_shared<APISession>(*this)));
}

void WSServer::on_close(websocketpp::connection_hdl hdl)
{
    INFO("WebSocket connection closed.");
    connection_session_.erase(hdl);
}

void WSServer::on_message(websocketpp::connection_hdl hdl, Server::message_ptr msg)
{
    ASSERT_LOG(connection_session_.find(hdl) != connection_session_.end(),
               "Cannot retrieve API pointer from connection handle.");
    auto session_handle = connection_session_.find(hdl)->second;

    // todo maybe parse first so be we can have better error handling.
    auto db_req_counter = dbsrv_->operation_count();
    Audit::IWSAPICallPtr audit;
    boost::optional<ServerMessage> response = ServerMessage();
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
        response->status_code   = APIStatusCode::DATABASE_ERROR;
        response->status_string = e.what();
        send_message(hdl, *response);
        return;
    }
    try
    {
        audit->event_mask(Audit::EventType::WSAPI_CALL);
        audit->author(session_handle->current_user());
        auto ws_connection_ptr = srv_.get_con_from_hdl(hdl);
        ASSERT_LOG(ws_connection_ptr, "No websocket connection object from handle.");
        audit->source_endpoint(ws_connection_ptr->get_remote_endpoint());

        // todo careful potential DDOS as we store the full content without checking
        // for now.
        audit->request_content(msg->get_payload());
        // Parse request, and copy uuid/method into the audit object.
        req = json::parse(msg->get_payload());
        INFO("Incoming payload: \n" << req.dump(4));

        ClientMessage input_msg = parse_request(req);
        audit->uuid(input_msg.uuid);
        audit->method(input_msg.type);
        dbsrv_->update(*audit); // update audit with new info
        response = handle_request(session_handle, input_msg, audit);
    }
    catch (const std::invalid_argument &e)
    {
        // JSON parse error
        response->status_code   = APIStatusCode::MALFORMED;
        response->status_string = "Failed to parse JSON.";
    }
    catch (const MalformedMessage &e)
    {
        response->status_code   = APIStatusCode::MALFORMED;
        response->status_string = e.what();
    }

    if (response)
    {
        audit->database_operations(
            static_cast<uint16_t>(dbsrv_->operation_count() - db_req_counter));
        finalize_audit(audit, *response);
        send_message(hdl, *response);
    }
}

void WSServer::run(const std::string &interface, uint16_t port)
{
    INFO("WebSockAPI server thread id is " << gettid());

    boost::asio::ip::tcp::endpoint endpoint(
        boost::asio::ip::address::from_string(interface), port);
    srv_.listen(endpoint);
    srv_.start_accept();

    get_service_registry().register_service<Service>(
        std::make_unique<Service>(*this));
    srv_.run();
    bool ok = get_service_registry().unregister_service<Service>();
    if (!ok)
        WARN("Failed to unregister the WebSockAPI::Service.");
}

void WSServer::start_shutdown()
{
    srv_.get_io_service().post([this]() {
        srv_.stop_listening();
        for (auto con_session : connection_session_)
        {
            websocketpp::lib::error_code ec;
            srv_.close(con_session.first, 0, "bye", ec);
            if (ec.value() == websocketpp::error::value::invalid_state)
            {
                // Maybe the connection is already dead at this point.
                // We can then safely ignore this error code.
                continue;
            }
            ASSERT_LOG(ec.value() == 0,
                       BUILD_STR("Websocketpp error: " << ec.message()));
        }
    });
}

APIAuth &WSServer::auth()
{
    return auth_;
}

boost::optional<json> WSServer::dispatch_request(APIPtr api_handle,
                                                 const ClientMessage &in,
                                                 Audit::IAuditEntryPtr audit)
{
    auto asio_handler = asio_handlers_.find(in.type);
    if (asio_handler != asio_handlers_.end())
    {
        RequestContext ctx{.session      = api_handle,
                           .dbsrv        = dbsrv_,
                           .server       = *this,
                           .original_msg = in,
                           .security_ctx = api_handle->security_context(),
                           .audit        = audit};
        // Will block the current thread until the response has been built.
        return asio_handler->second(ctx);
    }

    auto external_handler_key = external_handlers_.find(in.type);
    if (external_handler_key != external_handlers_.end())
    {
        // Forward the message to the WebSockAPIModule thread, which will
        // itself forward to the corresponding module.

        // We reconstruct the JSON object to send to the module that will
        // handle the message.
        json reconstructed = {
            {"uuid", in.uuid}, {"type", in.type}, {"content", in.content}};
        zmqpp::message msg;
        msg << external_handler_key->second << audit->id()
            << api_handle->connection_identifier() << reconstructed.dump(4);
        bool sent = to_module_->send(msg, true);
        ASSERT_LOG(sent, "Internal send would block. Need to investigate.");
        return boost::none;
    }

    // A request is an "Unit-of-Work" for the application.
    // We create a default database session for the request.
    odb::session database_session;
    auto handler_factory = individual_handlers_.find(in.type);
    api_handle->hook_before_request();

    if (handler_factory != individual_handlers_.end())
    {
        RequestContext ctx{.session      = api_handle,
                           .dbsrv        = dbsrv_,
                           .server       = *this,
                           .original_msg = in,
                           .security_ctx = api_handle->security_context(),
                           .audit        = audit};

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
        RequestContext ctx{.session      = api_handle,
                           .dbsrv        = dbsrv_,
                           .server       = *this,
                           .original_msg = in,
                           .security_ctx = api_handle->security_context(),
                           .audit        = audit};

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

boost::optional<ServerMessage> WSServer::handle_request(APIPtr api_handle,
                                                        const ClientMessage &msg,
                                                        Audit::IAuditEntryPtr audit)
{
    ServerMessage response;
    response.status_code = APIStatusCode::SUCCESS;
    response.content     = {};
    try
    {
        response.uuid = msg.uuid;
        response.type = msg.type;
        auto opt_json = dispatch_request(api_handle, msg, audit);
        if (opt_json)
        {
            response.content = *opt_json;
            return response;
        }
        return boost::none;
    }
    catch (...)
    {
        return ExceptionConverter().convert_merge(std::current_exception(),
                                                  response);
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
    for (const auto &connection_to_session : connection_session_)
    {
        const auto &session = connection_to_session.second;
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
            send_message(connection_to_session.first, msg);
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

void WSServer::register_external_handler(const std::string &name,
                                         const std::string &client_id)
{
    srv_.get_io_service().post([=]() {
        zmqpp::message msg;
        bool sent;
        if (has_handler(name))
        {
            // Handler already registered.
            WARN("An websocket handler named " << name << " is already registered.");
            msg << client_id << "KO";
        }
        else
        {
            external_handlers_[name] = client_id;
            msg << client_id << "OK";
        }
        sent = to_module_->send(msg, true);
        ASSERT_LOG(sent,
                   "Internal messaging would block. Something is probably wrong.");
    });
}

bool WSServer::has_handler(const std::string &name) const
{
    return handlers_.count(name) || individual_handlers_.count(name) ||
           crud_handlers_.count(name) || external_handlers_.count(name) ||
           asio_handlers_.count(name);
}

void WSServer::send_external_message(const std::string &connection_identifier,
                                     const std::string &content)
{
    srv_.get_io_service().post([=]() {
        auto connection_handle = find_connection(connection_identifier);
        if (connection_handle.lock() && srv_.get_con_from_hdl(connection_handle))
        {
            srv_.send(connection_handle, content, websocketpp::frame::opcode::text);
        }
        else
        {
            WARN("Message for " << Colorize::cyan(connection_identifier)
                                << " cannot be delivered because the connection "
                                   "doesn't exist anymore.");
        }
    });
}

websocketpp::connection_hdl
WSServer::find_connection(const std::string &connection_identifier) const
{
    // Implementation is probably: 1) slow. 2) fast enough for now.

    for (const auto &con_to_session : connection_session_)
    {
        if (con_to_session.second->connection_identifier() == connection_identifier)
            return con_to_session.first;
    }
    return websocketpp::connection_hdl();
}

void WSServer::finalize_audit(const Audit::IWSAPICallPtr &audit, ServerMessage &msg)
{
    try
    {
        db::MultiplexedTransaction t(dbsrv_->db()->begin());
        // If something went wrong while processing the request, the audit object
        // may need to be reload. We might as well reload it every time.
        audit->reload();
        // Update audit value.
        audit->uuid(msg.uuid);
        audit->method(msg.type);
        audit->status_code(msg.status_code);
        audit->status_string(msg.status_string);
        // audit->response_content(msg.content.dump(4));
        audit->finalize();
        t.commit();
    }
    catch (const odb::exception &e)
    {
        WARN("Database Error in WServer::on_message. Failed to persist final audit: "
             << e.what());
        msg.status_code   = APIStatusCode::DATABASE_ERROR;
        msg.status_string = e.what();
        return;
    }
}

void WSServer::send_to_module(const std::string &module_name, zmqpp::message msg)
{
    msg.push_front(module_name);
    msg.push_front("SEND_TO_MODULE");

    // Note that this socket doesn't send directly to the target module.
    // The message will be dispatched by the WebSockAPI thread.
    bool ret = to_module_->send(msg, true);
    ASSERT_LOG(ret, "Internal send would have blocked.");
}

bool WSServer::ASIO_REGISTER_HANDLER(const Service::WSHandler &handler,
                                     const std::string &name)
{
    DEBUG("Scheduling ASIO-based-handler registration. (name: " << name << ')');
    std::packaged_task<bool()> pt([=]() {
        if (has_handler(name))
            return false;
        DEBUG("Performing registration of ASIO-based-handler. (name: " << name
                                                                       << ')');
        asio_handlers_[name] = handler;
        return true;
    });
    std::future<bool> future = pt.get_future();

    srv_.get_io_service().post([&]() { pt(); });
    return future.get();
}
