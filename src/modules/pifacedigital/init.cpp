/*
    Copyright (C) 2014-2022 Leosac

    This file is part of Leosac.

    Leosac is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac is distrsrc/hardware/HardwareService.hppibuted in the hope that it will be
   useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "PFDigitalModule.hpp"
#include "tools/log.hpp"
#include <boost/property_tree/ptree.hpp>
#include <zmqpp/zmqpp.hpp>

extern "C" {
const char *get_module_name()
{
    return "PIFACEDIGITAL_GPIO";
}
}

using namespace Leosac::Module::Piface;

/**
* This function is the entry point of the PifaceDigital module.
* @see PFDigitalModule description.
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
    {
        WARN("Setting realtime priority for "
             << get_module_name() << " module failed. "
             << "The program will keep running but could encounter issues like "
                "missing some Wiegand bits.");
    }

    return Leosac::Module::start_module_helper<PFDigitalModule>(pipe, cfg, zmq_ctx,
                                                                utils);
}
