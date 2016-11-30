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
#include "zmqpp/zmqpp.hpp"

/**
* Implements a message bus (running in its own thread) that use 2 sockets.
*
* PULL socket to receive message from client (available at `inproc://zmq-bus-pull`)
* PUB socket to publish everything it received (available at `inproc://zmq-bus-pub`)
*/
class MessageBus
{
  public:
    MessageBus(zmqpp::context &ctx);
    ~MessageBus();

  private:
    zmqpp::actor *actor_;

    /**
    * The method that will be run in the child thread.
    * Pipe is a PAIR socket back to ZmqBus object.
    */
    bool run(zmqpp::socket *pipe);

    zmqpp::context &ctx_;

    zmqpp::socket *pub_;
    zmqpp::socket *pull_;


    void handle_pipe(zmqpp::socket *pipe);
    void handle_pull();

    bool running_;
};