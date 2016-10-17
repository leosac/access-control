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

#include "modules/websock-api/Messages.hpp"

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
/**
 * This class is dedicated to converting exception thrown
 * by various code into a ServerMessage that can be sent
 * back to a websocket client.
 */
class ExceptionConverter
{
  public:
    /**
     * Convert the exception_ptr to a ServerMessage and merge it
     * with an other message.
     *
     * @param ptr
     * @param msg The "current" message. We will copy type/uuid from it.
     */
    ServerMessage convert_merge(const std::exception_ptr &ptr,
                                const ServerMessage &msg);

  private:
    virtual ServerMessage convert_impl(const std::exception_ptr &ptr);
};
}
}
}
