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

#include "modules/ws2/WS2Service.hpp"
#include "modules/ws2/MyWSServer.hpp"

namespace Leosac
{
namespace Module
{

namespace WS2
{

bool WS2Service::register_coroutine_handler(const std::string &message_type,
                                            CoroutineRequestHandler handler,
                                            MessageProcessingPolicy policy)
{
    return handler_manager_.register_coroutine_handler(message_type, handler,
                                                       policy);
}

bool WS2Service::register_coroutine_handler(const std::string &message_type,
                                            CoroutineRequestHandler handler)
{
    return handler_manager_.register_coroutine_handler(message_type, handler);
}

bool WS2Service::register_handler(const std::string &message_type,
                                  RequestHandler handler,
                                  MessageProcessingPolicy policy)
{
    return handler_manager_.register_handler(message_type, handler, policy);
}

bool WS2Service::register_handler(const std::string &message_type,
                                  RequestHandler handler)
{
    return handler_manager_.register_handler(message_type, handler);
}
}
}
}