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
#include "TestAndResetModule.hpp"

using namespace Leosac::Module::TestAndReset;

extern "C"
{
const char *get_module_name()
{
    return "TEST_AND_RESET";
}
}

/**
* This function is the entry point of the Test And Reset module.
*/
extern "C" __attribute__((visibility("default")))
bool start_module(zmqpp::socket *pipe,
                  boost::property_tree::ptree cfg,
                  zmqpp::context &zmq_ctx,
                  Leosac::Scheduler &sched)
{
    return Leosac::Module::start_module_helper<TestAndResetModule>(pipe, cfg, zmq_ctx,
                                                                   sched);
}
