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
#include "InstrumentationModule.hpp"

using namespace Leosac::Module::Instrumentation;

InstrumentationModule::InstrumentationModule(zmqpp::context &ctx,
        zmqpp::socket *pipe,
        const boost::property_tree::ptree &cfg,
        CoreUtilsPtr utils) :
        BaseModule(ctx, pipe, cfg, utils),
        bus_push_(ctx, zmqpp::socket_type::push),
        controller_(ctx, zmqpp::socket_type::router)
{
    std::string bind_str = "ipc://" + config_.get_child("module_config").get<std::string>("ipc_endpoint");
    controller_.bind(bind_str);
    INFO("Binding to: " << bind_str);
    bus_push_.connect("inproc://zmq-bus-pull");
    reactor_.add(controller_, std::bind(&InstrumentationModule::handle_command, this));
}

void InstrumentationModule::handle_command()
{
    INFO("COMMAND AVAILABLE");
    zmqpp::message msg;

    controller_.receive(msg);
    std::string identity;
    std::string str;

    msg >> identity >> str;
    if (str == "GPIO")
    {
        handle_gpio_command(&msg);
    }
    else
    {
        // since this is a test/debug module, lets die if we receive bad input.
        assert(0);
    }
}

void InstrumentationModule::handle_gpio_command(zmqpp::message *str)
{
    assert(str);
    std::string cmd;
    std::string gpio_name;

    *str >> gpio_name >> cmd;
    DEBUG("GPIO CMD: " << gpio_name << ", " << cmd);
    if (cmd == "ON")
    {
        bus_push_.send(zmqpp::message() << ("S_" + gpio_name) << "ON");
    }
    else if (cmd == "OFF")
    {
        bus_push_.send(zmqpp::message() << ("S_" + gpio_name) << "OFF");
    }
    else if (cmd == "INT")
    {
        bus_push_.send(zmqpp::message() << std::string("S_INT:" + gpio_name));
    }
}
