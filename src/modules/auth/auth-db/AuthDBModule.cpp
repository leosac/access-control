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

#include "modules/auth/auth-db/AuthDBModule.hpp"
#include "core/CoreUtils.hpp"
#include "core/kernel.hpp"
#include "modules/auth/auth-db/AuthDBInstance.hpp"
#include <boost/property_tree/ptree.hpp>

using namespace Leosac;
using namespace Leosac::Module::Auth;

AuthDBModule::AuthDBModule(zmqpp::context& ctx, zmqpp::socket *pipe, 
                           const boost::property_tree::ptree &cfg, CoreUtilsPtr utils)
    : AsioModule(ctx, pipe, cfg, utils)
{
    process_config();
}

AuthDBModule::~AuthDBModule() {}

void AuthDBModule::on_service_event(const service_event::Event &event) {}

void AuthDBModule::process_config() {
    boost::property_tree::ptree auth_db_cfg = config_.get_child("module_config");

    for (const auto &instance_node : auth_db_cfg.get_child("instances")) {
        boost::property_tree::ptree auth_instance_cfg = instance_node.second;
        std::string auth_ctx_name = auth_instance_cfg.get_child("name").data();
        std::string auth_target_name = auth_instance_cfg.get<std::string>("target", "");
        std::list<std::string> auth_sources_names;

        for (const auto &instance_subnode : auth_instance_cfg) {
            if (instance_subnode.first == "auth_source") {
                auth_sources_names.push_back(instance_subnode.second.data());
            }
        }

        if (!auth_target_name.empty()) {
            auth_target_name = utils_->kernel().config_manager().instance_name() + '.' + auth_target_name;
        }

        INFO("Config processed for AuthDB instance: " << auth_ctx_name);
        INFO("  - Target: " << auth_target_name);
        INFO("  - Sources: " << boost::algorithm::join(auth_sources_names, ", "));
    }
}

void AuthDBModule::setup_tables() {
    // TODO: Re-implement this back in the future
}

void AuthDBModule::setup_authenticators() {
    // TODO: Implement
}