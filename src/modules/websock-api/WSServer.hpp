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

#pragma once

#include "WebSockAPI.hpp"
#include "api/APIAuth.hpp"
#include "api/APISession.hpp"
#include "api/CRUDResourceHandler.hpp"
#include "api/MethodHandler.hpp"
#include "core/APIStatusCode.hpp"
#include "core/audit/AuditFwd.hpp"
#include "tools/db/db_fwd.hpp"
#include <set>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
using json = nlohmann::json;

/**
 * A message sent by the server to a client.
 */
struct ServerMessage
{
    APIStatusCode status_code;
    std::string status_string;
    std::string uuid;
    std::string type;
    json content;
};

/**
 * A message sent by a client to Leosac.
 */
struct ClientMessage
{
    std::string uuid;
    std::string type;
    json content;
};

class WSServer
{
  public:
    /**
     * @param database A (non-null) pointer to the
     * database.
     */
    WSServer(WebSockAPIModule &module, DBPtr database);

    using Server           = websocketpp::server<websocketpp::config::asio>;
    using ConnectionAPIMap = std::map<websocketpp::connection_hdl, APIPtr,
                                      std::owner_less<websocketpp::connection_hdl>>;

    void run(const std::string &interface, uint16_t port);

    Server srv_;

    /**
     * Start the process of shutting down the server.
     *
     * The server will stop listening for new connection and will
     * attempt to close existing one.
     *
     * @note This method can safely be called from an other thread.
     */
    void start_shutdown();

    /**
     * Retrieve the authentication helper.
     */
    APIAuth &auth();

    /**
     * Retrieve database handle
     */
    DBPtr db();

    /**
     * Retrieve database service pointer.
     */
    DBServicePtr dbsrv();

    /**
     * Retrieve the CoreUtils pointer.
     */
    CoreUtilsPtr core_utils();

    /**
     * Deauthenticate all the connections of `user`, except
     * the `exception` APISession.
     *
     * @param new_transaction If set to true, a new odb::transaction will be used to
     * delete the authentication tokens from the database. If this is false, the
     * currently active transaction is used, and it is the caller responsibility to
     * make
     * sure that the transaction will be commited.
     * Invoking this method with \new_transaction` being false and no currently
     * active
     * transaction is not allowed.
     */
    void clear_user_sessions(Auth::UserPtr user, APIPtr exception,
                             bool new_transaction = true);

  private:
    void on_open(websocketpp::connection_hdl hdl);

    void on_close(websocketpp::connection_hdl hdl);

    /**
     * A websocket message has been received.
     *
     * At this point, all error handling happens through the use of exception.
     * Non-exceptional event, such as:
     *   + Malformed packet
     *   + Unsuficient permission to perform a given API call
     *   + ...
     * are also reported through exceptions.
     *
     * While this may not be the best performance wise, it's
     * unlikely to be a bottleneck, but it helps keep things clean.
     *
     * @note This method is reponsible for saving the WSAPICall Audit event.
     */
    void on_message(websocketpp::connection_hdl hdl, Server::message_ptr msg);

    /**
     * Handle a request.
     *
     * Extract request header, set-up exception handler for api handler
     * invokation.
     */
    ServerMessage handle_request(APIPtr api_handle, const json &req,
                                 Audit::IAuditEntryPtr);

    /**
     * Create a ClientMessage object from a json request.
     */
    ClientMessage parse_request(const json &in);

    /**
     * Send a message over a connection.
     * @param hdl The connection
     * @param msg The message.
     */
    void send_message(websocketpp::connection_hdl hdl, const ServerMessage &msg);

    /**
     * An internal helper function to register a CRUD resource handler.
     *
     * The resource name will be expanded to create handler for multiple type of
     * requests:
     *     + "resource_name"_read
     *     + "resource_name"_create
     *     + ...
     *
     * @param resource_name
     * @param factory
     */
    void register_crud_handler(const std::string &resource_name,
                               CRUDResourceHandler::Factory factory);

    /**
     * Process a request from a client.
     */
    json dispatch_request(APIPtr api_handle, const ClientMessage &in,
                          Audit::IAuditEntryPtr);

    ConnectionAPIMap connection_api_;
    APIAuth auth_;

    /**
     * This maps (string) command name to API method.
     */
    std::map<std::string, json (APISession::*)(const json &)> handlers_;

    std::map<std::string, MethodHandler::Factory> individual_handlers_;

    std::map<std::string, CRUDResourceHandler::Factory> crud_handlers_;

    /**
     * Database service object.
     */
    DBServicePtr dbsrv_;

    /**
     * A reference to the module.
     *
     * The module is guaranteed to outlive the WSServer.
     */
    WebSockAPIModule &module_;
};
}
}
}
