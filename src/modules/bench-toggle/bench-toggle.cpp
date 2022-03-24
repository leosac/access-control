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

#include "hardware/facades/FGPIO.hpp"
#include "tools/log.hpp"
#include "zmqpp/zmqpp.hpp"
#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <zmqpp/context.hpp>
#include <zmqpp/message.hpp>

/**
* Send the request to the target and handle the response.
* Return false if no response in 1s.
*/
bool send_request(std::shared_ptr<zmqpp::socket> target, const std::string &cmd1)
{
    target->send(cmd1);

    zmqpp::poller p;

    p.add(*target.get(), zmqpp::poller::poll_in);
    if (!p.poll(1000))
    {
        INFO("No response from target (" << target << ")");
        return false;
    }

    zmqpp::message_t m;
    target->receive(m);

    std::string rep;
    m >> rep;
    INFO("response = " << rep);
    return true;
}

/**
* pipe is pipe back to module manager.
* this function is called in its own thread.
*
* do signaling when ready
*/
extern "C" __attribute__((visibility("default"))) bool
start_module(zmqpp::socket *pipe, boost::property_tree::ptree cfg,
             zmqpp::context &zmq_ctx)
{
    INFO("Bench-Toggle will start");
    std::string endpoint_to_bench =
        cfg.get_child("module_config").get_child("target").data();
    int itr      = cfg.get_child("module_config").get<int>("iterations");
    int wait_for = cfg.get_child("module_config").get<int>("pause");
    pipe->send(zmqpp::signal::ok);

    // fixme since module order initialization is not defined we need to wait.
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    std::shared_ptr<zmqpp::socket> sock(
        new zmqpp::socket(zmq_ctx, zmqpp::socket_type::req));

    sock->connect("inproc://" + endpoint_to_bench);
    INFO("should take about " << itr * wait_for << "ms to run");
    std::chrono::system_clock::time_point clock = std::chrono::system_clock::now();
    Leosac::Hardware::FGPIO my_gpio(zmq_ctx, endpoint_to_bench);
    for (int i = 0; i < itr; i++)
    {
        // send_request(sock, "TOGGLE");
        my_gpio.toggle();
        std::this_thread::sleep_for(std::chrono::milliseconds(wait_for));
    }
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now() - clock);
    DEBUG("TOOK " << elapsed.count() << "ms");

    INFO("Module Bench-Toggle shutting down");
    return true;
}
