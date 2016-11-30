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

#include "SysFsGpioModule.hpp"
#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <tools/log.hpp>
#include <zmqpp/context.hpp>
#include <zmqpp/message.hpp>

using namespace Leosac::Module::SysFsGpio;

extern "C" {
const char *get_module_name()
{
    return "SYSFS_GPIO";
}
}


/**
* Entry point of the SysFsGpio module.
* It provides a way to control GPIO through the sysfs kernel interface.
*/
extern "C" __attribute__((visibility("default"))) bool
start_module(zmqpp::socket *pipe, boost::property_tree::ptree cfg,
             zmqpp::context &zmq_ctx, Leosac::CoreUtilsPtr utils)
{
    // this thread need realtime priority so it doesn't miss interrupt.
    struct sched_param p;
    p.sched_priority = 90;
    int ret          = pthread_setschedparam(pthread_self(), SCHED_FIFO, &p);
    if (ret != 0)
        WARN("Cannot set realtime priority for SysFsGPIO module.");
    return Leosac::Module::start_module_helper<SysFsGpioModule>(pipe, cfg, zmq_ctx,
                                                                utils);
}
