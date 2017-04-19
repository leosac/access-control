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

#include "modules/ws2/WS2Fwd.hpp"
#include <boost/asio/spawn.hpp>

namespace Leosac
{
namespace Module
{

namespace WS2
{
class WS2Service
{
  public:
    WS2Service(HandlerManager &h)
        : handler_manager_(h)
    {
    }
    using RequestHandler =
        std::function<boost::optional<json>(const json &, ReqCtx)>;

    using CoroutineRequestHandler = std::function<boost::optional<json>(
        const json &, ReqCtx, boost::asio::yield_context)>;

    /**
     * @see HandlerManager::register_coroutine_handler()
     */
    bool register_coroutine_handler(const std::string &message_type,
                                    CoroutineRequestHandler handler,
                                    MessageProcessingPolicy policy);

    /**
      * @see HandlerManager::register_coroutine_handler()
      */
    bool register_coroutine_handler(const std::string &message_type,
                                    CoroutineRequestHandler handler);

    /**
     * @see HandlerManager::register_handler()
     */
    bool register_handler(const std::string &message_type, RequestHandler handler,
                          MessageProcessingPolicy policy);

    /**
     * @see HandlerManager::register_handler()
     */
    bool register_handler(const std::string &message_type, RequestHandler handler);

    HandlerManager &handler_manager_;
};
}
}
}
