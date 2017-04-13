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

#pragma once

#include "LeosacFwd.hpp"
#include "Parser.hpp"
#include "core/audit/AuditFwd.hpp"
#include "core/auth/AuthFwd.hpp"
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

class ReqCtx
{
  public:
    ~ReqCtx();

    ReqCtx(boost::asio::io_service &io)
        : io_service_(io)
    {
    }

    ReqCtx(const ReqCtx &o)
        : audit_(o.audit_)
        , sc_(o.sc_)
        , metadata_(o.metadata_)
        , connection_hdl_(o.connection_hdl_)
        , io_service_(o.io_service_)
    {
    }

    Audit::IAuditEntryPtr audit_;
    SecurityContextCPtr sc_;

    ConnectionMetadataPtr metadata_; // may be null
    websocketpp::connection_hdl connection_hdl_;
    boost::asio::io_service &io_service_;
};

enum class MessageProcessingPolicy
{
    /**
     * Message are enqueue on the source connection's
     * queue.
     * This means that no two coroutines processing a message
     * with QUEUED policy for the connection can be alive simultaneously.
     *
     * The coroutine handling the message runs on the connection's strand.
     */
    QUEUED,

    /**
     * The message shall be processed asynchronously. It may be processed
     * concurrently (but not in parallel) with a QUEUED message or
     * other ASYNC for a given connection.
     *
     * The coroutine handling the message runs on the connection's strand.
     */
    CONCURRENT,

    /**
     * Message processed with this policy must not require any information,
     * metadata or otherwise, from the connection the message originates from.
     *
     * The handler is run WITHOUT ANY strand.
     * The handler may block the thread.
     */
    PARALLEL
};

enum class HandlerType
{
    FUNCTION,
    COROUTINE
};

class MyWSServer;

/**
 * Manage message handler for the Websocket Server.
 *
 * This class must be thread safe because handler are looked up
 * by the worker thread.
 *
 * This class is tightly coupled with the MyWSServer class.
 */
class HandlerManager
{
    using RequestHandler =
        std::function<boost::optional<json>(const json &, ReqCtx)>;

    using CoroutineRequestHandler = std::function<boost::optional<json>(
        const json &, ReqCtx, boost::asio::yield_context)>;

  public:
    HandlerManager(MyWSServer &s);

    struct HandlerInfo
    {
        RequestHandler handler_;

        /**
         * If true, the `mpp_` always apply for this handler.
         */
        bool force_processing_policy_;

        /**
         * The processing policy to use for the message.
         * If force_processing_policy_ is true, always apply
         * this policy.
         */
        MessageProcessingPolicy mpp_;
    };

    struct CoroutineHandlerInfo
    {

        CoroutineRequestHandler handler_;

        /**
         * If true, the `mpp_` always apply for this handler.
         */
        bool force_processing_policy_;

        /**
         * The processing policy to use for the message.
         * If force_processing_policy_ is true, always apply
         * this policy.
         */
        MessageProcessingPolicy mpp_;
    };

    bool register_handler(const std::string &message_type, HandlerInfo hi);

    template <typename HandlerT>
    bool register_handler(const std::string &message_type, HandlerT handler)
    {
        std::lock_guard<decltype(mutex_)> lg(mutex_);
        HandlerManager::HandlerInfo hi;
        hi.handler_                 = handler;
        hi.force_processing_policy_ = false;

        if (has_handler(message_type))
            return false;

        handlers_[message_type] = hi;
        return true;
    }

    bool register_coroutine_handler(const std::string &message_type,
                                    CoroutineHandlerInfo chi);

    template <typename HandlerT>
    bool register_coroutine_handler(const std::string &message_type,
                                    HandlerT handler)
    {
        std::lock_guard<decltype(mutex_)> lg(mutex_);
        HandlerManager::CoroutineHandlerInfo hi;
        hi.handler_                 = handler;
        hi.force_processing_policy_ = false;

        if (has_handler(message_type))
            return false;

        coroutine_handlers_[message_type] = hi;
        return true;
    }

    /**
     * Retrieve the type of the Handler for a given type of message.
     */
    HandlerType get_handler_type(const std::string &message_type) const;

    /**
     * Retrieve the processing policy for a given message type.
     * This function returns true and store the policy in `out_policy` if
     * it is forced. Otherwise it returns false and `out_policy` is not
     * modified.
     *
     * @param out_policy Store the policy for the message type in `out_policy`
     * @return true if the policy is forced the message's type, false otherwise.
     */
    bool get_forced_processing_policy(const std::string &message_type,
                                      MessageProcessingPolicy &out_policy) const;

    /**
     * Invoked a function based handler for a message whose policy
     * is parallel.
     */
    void InvokeParallelFunctionHandler(ReqCtx rctx, const ClientMessage &msg);

    /**
 * Invoked a coroutine based handler for a message whose policy
 * is parallel.
 */
    void InvokeParallelCoroutineHandler(ReqCtx rctx, const ClientMessage &msg);

    void InvokeConcurrentCoroutineHandler(ReqCtx rctx, const ClientMessage &msg);
    void InvokeConcurrentFunctionHandler(ReqCtx rctx, const ClientMessage &msg);

    void InvokeQueuedFunctionHandler(ReqCtx rctx, const ClientMessage &msg);
    void InvokeQueuedCoroutineHandler(ReqCtx rctx, const ClientMessage &msg);

  private:
    /**
     * Check if we have a handler for a given message.
     *
     * @warning This method must be called with the lock held.
     */
    bool has_handler(const std::string &message_type) const;

    std::map<std::string, HandlerInfo> handlers_;
    std::map<std::string, CoroutineHandlerInfo> coroutine_handlers_;

    mutable std::recursive_mutex mutex_;
    MyWSServer &server_;
};


/**
 * Leosac's websocket server.
 *
 * This makes use of the Websocketpp library to provide
 * websocket implementation.
 *
 * This server class provide parallelism to the server by
 * owning multiple thread that processs requests.
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
     * For for QUEUED message on the connection, and starts
     * processing the next one, if any.
     *
     * @warning This MUST be called from within the strand
     * of the connection (because it access the ConnectionMetadata,
     * and it is not thread safe).
     */
    void process_next_queued_msg(ReqCtx rctx);

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
    void ProcessMessage(websocketpp::connection_hdl hdl, const ClientMessage &msg,
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
