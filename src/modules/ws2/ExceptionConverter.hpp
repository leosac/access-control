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

#include "modules/ws2/Parser.hpp"

namespace Leosac
{
namespace Module
{
namespace WS2
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

    /**
     * Create a ServerMessage response object from an exception.
     *
     * The UUID of the ServerMessage returned by this function
     * is set "00000000-0000-0000-0000-000000000000" if the ClientMessage
     * wasn't properly parsed.
     */
    ServerMessage create_response_from_error(const std::exception_ptr &ptr,
                                             const ClientMessage &msg);

  private:
    virtual ServerMessage convert_impl(const std::exception_ptr &ptr);
};
}
}
}
