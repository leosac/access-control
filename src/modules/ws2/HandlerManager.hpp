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

#include "modules/ws2/WS2Fwd.hpp"
#include <boost/asio/spawn.hpp>
#include <mutex>

namespace Leosac
{
namespace Module
{

namespace WS2
{

/**
 * Manage message handlers for the Websocket Server.
 *
 * The class provides public facilities to register handlers
 * for given message types.
 * It also provides facilities (destined to the MyWSServer)
 * to invoke handler based on a predetermined type and policy.
 *
 * @note This class is thread safe and is self protected by an
 * internal mutex.
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
}
}
}