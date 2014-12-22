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

#include <core/auth/Auth.hpp>
#include "DoormanInstance.hpp"
#include "tools/log.hpp"

using namespace Leosac::Module::Doorman;

DoormanInstance::DoormanInstance(zmqpp::context &ctx,
        std::string const &name,
        const std::vector<std::string> &auth_contexts,
        const std::vector<DoormanAction> &actions,
        int timeout) :
        name_(name),
        actions_(actions),
        timeout_(timeout),
        bus_sub_(ctx, zmqpp::socket_type::sub)
{
    bus_sub_.connect("inproc://zmq-bus-pub");
    for (auto &endpoint : auth_contexts)
        bus_sub_.subscribe("S_" + endpoint);

    for (auto &action : actions_)
    {
        if (targets_.count(action.target_))
            continue; // already have a socket to this target.

        // create socket (and connect them) to target
        zmqpp::socket target_socket(ctx, zmqpp::socket_type::req);
        target_socket.connect("inproc://" + action.target_);
        targets_.insert(std::make_pair(action.target_, std::move(target_socket)));
    }
}

zmqpp::socket &DoormanInstance::bus_sub()
{
    return bus_sub_;
}

void DoormanInstance::handle_bus_msg()
{
    zmqpp::message bus_msg;
    std::string auth_name; // name of the auth context that sent the message.
    Leosac::Auth::AccessStatus access_status;

    bus_sub_.receive(bus_msg);
    assert(bus_msg.parts() >= 2);
    bus_msg >> auth_name >> access_status;
    DEBUG("DOORMAN HERE");

    for (auto &action : actions_)
    {
        if (action.on_ != access_status)
            continue; // status doesn't match what we expected.
        DEBUG("ACTION (target = " << action.target_ << ")");
        zmqpp::message msg;
        for (auto &frame : action.cmd_)
        {
            // we try to convert argument to int. if it works we send as int64_t, otherwise as string
            bool err = false;
            int v = 0;
            try
            {
                v = std::stoi(frame);
            }
            catch (...)
            {
                err = true;
            }
            if (err)
                msg << frame;
            else
                msg << static_cast<int64_t>(v);
            DEBUG("would do : " << frame << " to target: " << action.target_);
        }
        command_send_recv(action.target_, std::move(msg));
    }
}

void DoormanInstance::command_send_recv(std::string const &target_name, zmqpp::message msg)
{
    zmqpp::socket &target_socket = targets_.at(target_name);
    zmqpp::message response;

    target_socket.send(msg);
    target_socket.receive(response);

    std::string req_status;
    response >> req_status;

    if (req_status != "OK")
    {
        WARN("Command failed :(");
    }
}
