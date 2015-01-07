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

#include <memory>
#include <zmqpp/message.hpp>
#include <boost/property_tree/ptree.hpp>
#include <tools/log.hpp>
#include <zmqpp/context.hpp>
#include <zmqpp/poller.hpp>
#include "zmqpp/actor.hpp"
#include "MonitorModule.hpp"

using namespace Leosac::Module::Monitor;

struct logger_guard
{

    logger_guard(const std::initializer_list<std::string> &s) :
            logger_names(s)
    {
    }

    ~logger_guard()
    {
        for (const auto &l : logger_names)
            spdlog::drop(l);
    }

    std::vector<std::string> logger_names;
};

extern "C"
{
const char *get_module_name()
{
    return "MONITOR";
}
}

/**
* This is the entry point of the Monitor module.
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    logger_guard g({"system_bus_event", "monitor_stdout"});
    {
        MonitorModule module(zmq_ctx, pipe, cfg);
        pipe->send(zmqpp::signal::ok);
        INFO("Module Monitor initiliazed.");

        module.run();

        INFO("Module Monitor shutting down.");
    }
    return true;
}
