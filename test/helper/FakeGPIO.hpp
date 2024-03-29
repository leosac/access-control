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

#include "zmqpp/zmqpp.hpp"

namespace Leosac
{
namespace Test
{
namespace Helper
{
/**
* A test helper class that emulate a GPIO pin.
* It doesn't support the full command set.
*/
class FakeGPIO
{
  public:
    FakeGPIO(zmqpp::context &ctx, const std::string &name);

    FakeGPIO(const FakeGPIO &) = delete;

    /**
    * Emulate an interrupt by writing to the message bus
    */
    void interrupt();

    /**
    * if you want the GPIO in "passive" mode (ie to watch event generated BY the
    * device), run
    * this with a zmqpp::actor.
    */
    bool run(zmqpp::socket *pipe);

  private:
    // send current state to socket
    void send_state();

    void handle_cmd();

    zmqpp::context &ctx_;
    std::string name_;
    zmqpp::socket push_;

    /**
    * Receive command here.
    */
    zmqpp::socket rep_;

    bool value_;
};
}
}
}
