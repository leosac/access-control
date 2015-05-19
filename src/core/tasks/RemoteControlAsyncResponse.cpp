/*
    Copyright (C) 2014-2015 Islog

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

#include <tools/log.hpp>
#include "RemoteControlAsyncResponse.hpp"

using namespace Leosac;
using namespace Leosac::Tasks;

bool RemoteControlAsyncResponse::do_run()
{
    return socket_.send(msg_);
}

RemoteControlAsyncResponse::RemoteControlAsyncResponse(const std::string identity,
                                                       const zmqpp::message_t &msg,
                                                       zmqpp::socket_t &socket) :
        socket_(socket)
{
    INFO("Creating RemoteControlAsyncResponse task. Guid = " << get_guid());
    msg_ = msg.copy();
    msg_.push_front(identity);
}
