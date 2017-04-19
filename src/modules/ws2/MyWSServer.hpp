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

/**
 * The available threading policies wrt message handle.
 *
 * Each message type can force a message policy onto
 * its handler. If no policy is forced, we fallback
 * on the connection-level processing policy (whose default
 * is SERIAL).
 */
enum class MessageProcessingPolicy
{
    /**
     * Message are enqueued on the source connection's queue.
     * This means that no two coroutines or handler processing a message
     * with SERIAL policy for the connection can *exist* simultaneously.
     *
     * The coroutine or handler responsible for the message runs
     * on the connection's strand.
     *
     * Messages with this policy are therefore processed in the order
     * in which they are read from the socket.
     */
    SERIAL,

    /**
     * The message can be processed concurrently to other messages
     * with a CONCURRENT policy.
     *
     * It may be processed concurrently (but not in parallel) with a message
     * having a SERIAL processing policy.
     *
     * Handlers with CONCURRENT policy executes on the strand of the connection
     * from which the message originates. This means that no two CONCURRENT
     * handler (from a given connection) can *run* at the same time on multiple
     * threads.
     */
    CONCURRENT,

    /**
     * The PARALLEL policy doesn't impose additional constraints (such
     * as strand or queueing) to the messages that use it.
     *
     * The originating connection's metadatas ARE NOT available to
     * handlers running in PARALLEL mode. This is because ConnectionMetadata
     * objects are not thread-safe.
     *
     * The handler runs in any of the worker threads and DOES NOT
     * use a strand.
     */
    PARALLEL
};

/**
 * This enum describe the "type" of message handlers.
 *
 * Simply put, a message handler can either be a normal
 * callable (FUNCTION), or a coroutine.
 */
enum class HandlerType
{
    /**
     * Any normal callable object.
     */
    FUNCTION,

    /**
     * A callable that can behave like a coroutine.
     * COROUTINE handlers accept an additional parameter,
     * the yield context.
     */
    COROUTINE
};

class MyWSServer;

/**
 * Manage message handlers for the Websocket Server.
 *
 * The class provides public facilities to register handlers
 * for given message types.
 * It also provides facilities (destined to the MyWSServer)
 * to invoke handler based on a predetermined type and policy.
 */
class HandlerManager
{
    using RequestHandler =
        std::function<boost::optional<json>(const json &, ReqCtx)>;

    using CoroutineRequestHandler = std::function<boost::optional<json>(
        const json &, ReqCtx, boost::asio::yield_context)>;

  public:
    HandlerManager(MyWSServer &s);

    /**
     * Register a function backed handler using a forced processing
     * policy, `policy`.
     */
    bool register_handler(const std::string &message_type, RequestHandler handler,
                          MessageProcessingPolicy policy)
    {
        HandlerManager::HandlerInfo hi;
        hi.handler_                 = handler;
        hi.force_processing_policy_ = true;
        hi.mpp_                     = policy;

        return register_handler(message_type, hi);
    }

    /**
     * Register a function backed handler.
     * The handler doesn't force the processing policy.
     */
    bool register_handler(const std::string &message_type, RequestHandler handler)
    {
        HandlerManager::HandlerInfo hi;
        hi.handler_                 = handler;
        hi.force_processing_policy_ = false;

        return register_handler(message_type, hi);
    }

    /**
     * Register a coroutine based handler for the message.
     * The handler will run with the processing policy `policy`.
     */
    bool register_coroutine_handler(const std::string &message_type,
                                    CoroutineRequestHandler handler,
                                    MessageProcessingPolicy policy)
    {
        HandlerManager::CoroutineHandlerInfo hi;
        hi.handler_                 = handler;
        hi.force_processing_policy_ = true;
        hi.mpp_                     = policy;

        return register_coroutine_handler(message_type, hi);
    }

    /**
     * Register a coroutine based handler for the message.
     * The handler doesn't force the processing policy.
     */
    bool register_coroutine_handler(const std::string &message_type,
                                    CoroutineRequestHandler handler)
    {
        HandlerManager::CoroutineHandlerInfo hi;
        hi.handler_                 = handler;
        hi.force_processing_policy_ = false;

        return register_coroutine_handler(message_type, hi);
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
     * Below are handlers invocations methods.
     * We have a total of 6 methods, one per combination
     * of HandlerType and MessageProcessingPolicy.
     */

    /**
     * Invoke a function based handler for a message whose policy
     * is PARALLEL.
     */
    void invoke_parallel_fct_handler(ReqCtx rctx, const ClientMessage &msg);

    /**
     * Invoke a coroutine based handler for a message whose policy
     * is PARALLEL.
     */
    void invoke_parallel_coro_handler(ReqCtx rctx, const ClientMessage &msg);

    /**
     * Invoke a function handler for a message processed with
     * CONCURRENT policy.
     */
    void invoke_concurrent_fct_handler(ReqCtx rctx, const ClientMessage &msg);

    /**
     * Invoke a coroutine handler for a message processed with
     * CONCURRENT policy.
     */
    void invoke_concurrent_coro_handler(ReqCtx rctx, const ClientMessage &msg);

    /**
     * Invoke a function handler for a message processed using a SERIAL
     * policy
     */
    void invoke_serial_fct_handler(ReqCtx rctx, const ClientMessage &msg);

    /**
     * Invoke a coroutine handler for a message processed using a SERIAL
     * policy
     */
    void invoke_serial_coro_handler(ReqCtx rctx, const ClientMessage &msg);

  private:
    /**
     * Check if we have a handler for a given message.
     *
     * @warning This method must be called with the lock held.
     */
    bool has_handler(const std::string &message_type) const;

    /**
     * Store information wrt a function backed handler.
     */
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

    /**
     * Store information wrt coroutine backed handler.
     */
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

    /**
     * Register a handler for `message_type`.
     */
    bool register_handler(const std::string &message_type, HandlerInfo hi);

    /**
     * Register a coroutine handler for `message_type`.
     */
    bool register_coroutine_handler(const std::string &message_type,
                                    CoroutineHandlerInfo chi);

    /**
     * Call the user handler, and wrap its invokation in try catch
     * that will convert exception into WebSocket message to report
     * the error to the end-user.
     */
    void do_invoke_coro_handler(ReqCtx rctx, const ClientMessage &msg,
                                CoroutineRequestHandler handler,
                                boost::asio::yield_context yc);

    /**
     * @see do_invoke_coro_handler
     */
    void do_invoke_fct_handler(ReqCtx rctx, const ClientMessage &msg,
                               RequestHandler handler);


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
