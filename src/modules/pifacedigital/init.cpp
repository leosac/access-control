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

#include <zmqpp/zmqpp.hpp>
#include <boost/property_tree/ptree.hpp>
#include <tools/log.hpp>
#include "PFDigitalModule.hpp"

extern "C"
{
const char *get_module_name()
{
    return "PIFACEDIGITAL_GPIO";
}
}

/**
* This function is the entry point of the PifaceDigital module.
* @see PFDigitalModule description.
*/
extern "C" __attribute__((visibility("default"))) bool start_module(zmqpp::socket *pipe,
        boost::property_tree::ptree cfg,
        zmqpp::context &zmq_ctx)
{
    Leosac::Module::Piface::PFDigitalModule module(cfg, pipe, zmq_ctx);

    INFO("Module PFDigital initiliazed.");
    pipe->send(zmqpp::signal::ok);

    // this thread need realtime priority so it doesn't miss interrupt.
    struct sched_param p;
    p.sched_priority = 90;
    int ret = pthread_setschedparam(pthread_self(), SCHED_FIFO, &p);
    assert(ret == 0);

    module.run();

    INFO("Module PFDigital shutting down.");
    return true;
}