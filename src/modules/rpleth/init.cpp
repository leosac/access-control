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

/**
* \file init.cpp
* \brief init function for returning a module class to the core
*/

#include "RplethModule.hpp"
#include "tools/log.hpp"
#include <zmqpp/zmqpp.hpp>

using namespace Leosac::Module::Rpleth;

extern "C" {
const char *get_module_name()
{
    return "RPLETH";
}
}

/**
* This function is the entry point of the Rpleth module.
*/
extern "C" __attribute__((visibility("default"))) bool
start_module(zmqpp::socket *pipe, boost::property_tree::ptree cfg,
             zmqpp::context &zmq_ctx, Leosac::CoreUtilsPtr utils)
{
    return Leosac::Module::start_module_helper<RplethModule>(pipe, cfg, zmq_ctx,
                                                             utils);
}
