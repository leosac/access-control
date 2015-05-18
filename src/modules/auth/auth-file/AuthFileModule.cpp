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

#include "AuthFileModule.hpp"
#include "core/CoreUtils.hpp"
#include "core/kernel.hpp"

using namespace Leosac::Module::Auth;

AuthFileModule::AuthFileModule(zmqpp::context &ctx,
        zmqpp::socket *pipe,
        const boost::property_tree::ptree &cfg,
        CoreUtilsPtr utils) :
        BaseModule(ctx, pipe, cfg, utils)
{
    process_config();

    for (auto authenticator : authenticators_)
    {
        reactor_.add(authenticator->bus_sub(),
                std::bind(&AuthFileInstance::handle_bus_msg, authenticator));
    }
}

AuthFileModule::~AuthFileModule()
{
}

void AuthFileModule::process_config()
{
    boost::property_tree::ptree module_config = config_.get_child("module_config");

    for (auto &node : module_config.get_child("instances"))
    {
        boost::property_tree::ptree auth_instance_cfg   = node.second;
        std::string auth_ctx_name                       = auth_instance_cfg.get_child("name").data();
        std::string config_file                         = auth_instance_cfg.get_child("config_file").data();
        std::string auth_target_name                    = auth_instance_cfg.get<std::string>("target", "");
        std::list<std::string> auth_sources_names;

        for (const auto &subnode : auth_instance_cfg)
        {
            if (subnode.first == "auth_source")
                auth_sources_names.push_back(subnode.second.data());
        }

        if (!auth_target_name.empty())
            auth_target_name = utils_->kernel().instance_name();

        INFO("Creating AuthFile instance " << auth_ctx_name << ". Target door = " << auth_target_name);
        authenticators_.push_back(AuthFileInstancePtr(new AuthFileInstance(ctx_,
                auth_ctx_name,
                auth_sources_names,
                auth_target_name,
                config_file)));
    }
}

void AuthFileModule::dump_additional_config(zmqpp::message *out) const
{
    assert(out);

    for (auto &ptr : authenticators_)
    {
        *out << ptr->auth_file_name();
        *out << ptr->auth_file_content();
    }
}
