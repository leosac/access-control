/*
    Copyright (C) 2014-2017 Leosac

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

#include "LeosacFwd.hpp"
#include "Parser.hpp"
#include "core/audit/AuditFwd.hpp"
#include "core/auth/AuthFwd.hpp"
#include "modules/ws2/HandlerManager.hpp"
#include "modules/ws2/WS2Fwd.hpp"
#include "tools/db/db_fwd.hpp"
#include <boost/asio/spawn.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

namespace Leosac
{
namespace Module
{
/**
 * WS2 design goal:
 *      + Good scalability
 *      + Low latency
 *
 * Design:
 *
 *     * WS handling (json parsing, etc) is done on a thread pool
 *      backed by ASIO. Websocketpp server run on this io_service
 *      that runs on multiple thread.
 *
 *      * Websocket worker may block thread.
 *
 */
namespace WS2
{


/**
 * Leosac's websocket server.
 *
 * This makes use of the Websocketpp library to provide
 * websocket implementation.
 *
 * This server class provide parallelism to the server by
 * owning multiple thread that process requests.
 *
 * This object lives in its parent thread but doesn't perform activity
 * in that thread.
 */
class MyWSServer
{
  public:
    MyWSServer(const std::string &interface, uint16_t port, CoreUtilsPtr utils);

    ~MyWSServer();

    static constexpr int nb_worked_thread = 5;

    /**
     * This module explicitly expose CoreUtils to other
     * object in the module.
     */
    CoreUtilsPtr core_utils();

    /**
     * Start the process of shutting down the server.
     *
     * The server will stop listening for new connection and will
     * attempt to close existing one.
     *
     * @note This method can safely be called from an other thread.
     */
    void start_shutdown();

    using WebsocketPPServer = websocketpp::server<websocketpp::config::asio>;

  private:
    void on_open(websocketpp::connection_hdl hdl);

    void on_close(websocketpp::connection_hdl hdl);

    void on_message(websocketpp::connection_hdl hdl,
                    WebsocketPPServer::message_ptr msg);

    /**
     * Determine the processing policy that apply to this message.
     *
     * This is based on the type of message and possibly some state in the
     * connection metadata.
     */
    MessageProcessingPolicy
    determine_processing_policy(const ConnectionMetadataPtr &md,
                                const ClientMessage &in) const;

    /**
     * Mark the connection as ready for the next SERIAL message
     * and schedule a callable to process the next pending SERIAL
     * message, if any.
     *
     * @warning This MUST be called from within the strand
     * of the connection (because it access the ConnectionMetadata,
     * and it is not thread safe).
     */
    void process_next_serial_msg(const ConnectionMetadataPtr &md);

    /**
     * Send a ServerMessage to a client.
     *
     * This function must be called from one of the worker thread.
     */
    void send_server_message(websocketpp::connection_hdl hdl,
                             const ServerMessage &msg);

    /**
     * Process a message originating from `hdl` with the processing
     * policy `policy`.
     *
     * This method runs on the originating connection's strand.
     *
     * @param serial_next A boolean indicating that the message to be processed
     * in next in queue. This apply only when the processing policy is QUEUED.
     * If this boolean in true, it means the message was already queued and
     * so we don't check and try to put it in the queue again.
     */
    void process_msg(websocketpp::connection_hdl hdl, const ClientMessage &msg,
                     MessageProcessingPolicy policy, bool serial_next);

    /**
     * Retrieve our metadata associated with the connection handle.
     *
     * The program assert() if metadata cannot be found.
     */
    ConnectionMetadataPtr
    metadata_from_connection_hdl(websocketpp::connection_hdl hdl);

    WebsocketPPServer srv_;

    /**
     * Per connection metadata.
     */
    std::map<websocketpp::connection_hdl, ConnectionMetadataPtr,
             std::owner_less<websocketpp::connection_hdl>>
        metadatas_;

    /**
     * The IO service that handle websocket activity and
     * websocket messages processing.
     */
    boost::asio::io_service io_service_;

    /**
     * Threads that call io_service_'s run().
     */
    std::vector<std::unique_ptr<std::thread>> worker_thread_;

    /**
     * Lock used to protect access to the metadatas map from
     * the multiple worker thread.
     */
    mutable std::mutex metadata_mutex_;

    HandlerManager handler_manager_;
    friend class HandlerManager;

    DBServicePtr dbsrv_;

    // todo.
    bool audit_all{false};
};
}
}
}
