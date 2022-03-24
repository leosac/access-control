/*
    Copyright (C) 2014-2022 Leosac

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

#include "core/tasks/Task.hpp"
#include <zmqpp/zmqpp.hpp>

namespace Leosac
{
namespace Tasks
{
/**
 * This task represent an asynchronous response that shall
 * be sent over the Remote Control router socket.
 */
class RemoteControlAsyncResponse : public Task
{
  public:
    RemoteControlAsyncResponse(const std::string identity,
                               const zmqpp::message_t &msg, zmqpp::socket_t &socket);

  private:
    virtual bool do_run();

    zmqpp::message_t msg_;
    zmqpp::socket_t &socket_;
};
}
}