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

#include "MessageBus.hpp"
#include "tools/ThreadUtils.hpp"

MessageBus::MessageBus(zmqpp::context &ctx)
    : ctx_(ctx)
    , pub_(nullptr)
    , pull_(nullptr)
    , running_(true)
{
    actor_ =
        new zmqpp::actor(std::bind(&MessageBus::run, this, std::placeholders::_1));
}

MessageBus::~MessageBus()
{
    delete actor_;
}

bool MessageBus::run(zmqpp::socket *pipe)
{
    Leosac::set_thread_name("message_bus");
    try
    {
        pub_ = new zmqpp::socket(ctx_, zmqpp::socket_type::pub);
        pub_->bind("inproc://zmq-bus-pub");

        pull_ = new zmqpp::socket(ctx_, zmqpp::socket_type::pull);
        pull_->bind("inproc://zmq-bus-pull");
    }
    catch (std::exception &e)
    {
        return false;
    }

    pipe->send(zmqpp::signal::ok);

    zmqpp::reactor reactor;

    reactor.add(*pull_, std::bind(&MessageBus::handle_pull, this));
    reactor.add(*pipe, std::bind(&MessageBus::handle_pipe, this, pipe));

    while (running_)
    {
        reactor.poll();
    }
    delete pull_;
    delete pub_;
    return true;
}

void MessageBus::handle_pipe(zmqpp::socket *pipe)
{
    zmqpp::signal sig;
    pipe->receive(sig);

    if (sig == zmqpp::signal::stop)
    {
        running_ = false;
    }
}

void MessageBus::handle_pull()
{
    zmqpp::message msg;

    pull_->receive(msg);
    pub_->send(msg);
}
