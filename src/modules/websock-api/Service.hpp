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

#include "ActionActionParam.hpp"
#include "LeosacFwd.hpp"
#include "modules/websock-api/RequestContext.hpp"
#include "modules/websock-api/WebSockFwd.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/log.hpp"
#include <boost/asio/io_service.hpp>
#include <boost/optional.hpp>
#include <future>

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{

/**
 * A service object provided by the Websocket module.
 *
 * @note This service is fully thread-safe.
 */
class Service
{
  public:
    Service(WSServer &server)
        : server_(server)
    {
    }

    using WSHandler = std::function<boost::optional<json>(const RequestContext &)>;

    /**
     * Register an handler that will be invoked by the io_service `io`.
     *
     * @note This function actually wraps the user-provided handler so that
     * it can be invoked normally from the websocket thread.
     */
    template <typename HandlerT>
    bool register_asio_handler(HandlerT &&handler, const std::string &type,
                               boost::asio::io_service &io)
    {
        // We want the `handler` callable to be executed in the current
        // thread/io_service.

        // We wrap the user handler into a packaged task that will be `post()`ed
        // onto the io_service (`io`) queue.

        WSHandler wrapped_handler =
            [&, handler](const RequestContext &req_ctx) -> boost::optional<json> {
            std::packaged_task<boost::optional<json>(const RequestContext &)> pt(
                [&](const RequestContext &req_ctx) { return handler(req_ctx); });
            auto future = pt.get_future();
            io.post([&]() { pt(req_ctx); });
            return future.get();
        };
        return register_typed_handler(wrapped_handler, type);
    }

    /**
     * Remove an asio handler by name.
     *
     * This should be called by module that register handlers before
     * the module is destroyed in order to avoid crash / race condition.
     *
     * If you still have registered handler when the module goes out of
     * scope, the websocket module will attempt to `post()` to a dangling
     * io_service, causing a crash.
     *
     * @note We assume everyone is nice and module won't remove each-other
     * handler.
     */
    void remove_asio_handler(const std::string &name);

    template <typename HandlerT>
    bool register_asio_handler_permission(HandlerT &&handler,
                                          const std::string &type,
                                          ActionActionParam permission,
                                          boost::asio::io_service &io)
    {
        auto wrapped = [handler, permission](const RequestContext &req_ctx) {
            req_ctx.security_ctx.enforce_permission(permission.first,
                                                    permission.second);
            return handler(req_ctx);
        };
        return register_asio_handler(wrapped, type, io);
    }

    template <typename HandlerT>
    bool register_asio_handler_permission(HandlerT &&handler,
                                          const std::string &type,
                                          SecurityContext::Action permission,
                                          boost::asio::io_service &io)
    {
        ActionActionParam aap;
        aap.first = permission;
        return register_asio_handler_permission(handler, type, aap, io);
    }

  private:
    /**
     * Register an handler that is ready to be invoked in the
     * websocket thread.
     */
    bool register_typed_handler(const WSHandler &handler, const std::string &type);

    WSServer &server_;
};
}
}
}
