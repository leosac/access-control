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

#include "AuthDBInstance.hpp"
#include "core/CoreUtils.hpp"
#include "tools/db/DBService.hpp"
#include "tools/log.hpp"
#include <boost/algorithm/string/join.hpp>
#include <zmqpp/zmqpp.hpp>

using namespace Leosac::Module::Auth;

AuthDBInstance::AuthDBInstance(zmqpp::context &ctx,
                               const std::string &auth_ctx_name,
                               const std::list<std::string> &auth_sources_names,
                               const std::string &auth_target_name,
                               CoreUtilsPtr core_utils)
    : db_service_(std::make_shared<DBService>(core_utils->database()))
    , bus_push_(ctx, zmqpp::socket_type::push)
    , bus_sub_(ctx, zmqpp::socket_type::sub)
    , name_(auth_ctx_name)
    , target_name_(auth_target_name)
    , core_utils_(core_utils)
{
    bus_push_.connect("inproc://zmq-bus-pull");
    bus_sub_.connect("inproc://zmq-bus-pub");
    bus_sub_.subscribe("KERNEL");
    
    for (const auto &auth_source : auth_sources_names) {
        bus_sub_.subscribe("S_" + auth_source);
    }

    INFO("AuthDB Instance (" << auth_ctx_name << ") and is subscribed to "
                             << boost::algorithm::join(auth_sources_names, ", "));
}

AuthDBInstance::~AuthDBInstance(){}