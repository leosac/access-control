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

#include <core/auth/Auth.hpp>
#include "DoormanModule.hpp"
#include "DoormanInstance.hpp"
#include "tools/log.hpp"

using namespace Leosac::Module::Doorman;
using namespace Leosac::Auth;

DoormanModule::DoormanModule(zmqpp::context &ctx,
        zmqpp::socket *pipe,
        const boost::property_tree::ptree &cfg) :
        BaseModule(ctx, pipe, cfg)
{
    process_config();

    for (auto doorman : doormen_)
    {
        reactor_.add(doorman->bus_sub(),
                std::bind(&DoormanInstance::handle_bus_msg, doorman));
    }
}

DoormanModule::~DoormanModule()
{
    for (auto doorman : doormen_)
    {
        delete doorman;
    }
}

void DoormanModule::process_config()
{
    boost::property_tree::ptree module_config = config_.get_child("module_config");

    for (auto &node : module_config.get_child("instances"))
    {
        // one doorman instance
        boost::property_tree::ptree cfg_doorman = node.second;

        std::vector<std::string> auth_ctx_names;
        std::vector<DoormanAction> actions;
        std::string doorman_name = cfg_doorman.get_child("name").data();
        int timeout = cfg_doorman.get<int>("timeout", 1000);

        for (auto &auth_contexts_node : cfg_doorman.get_child("auth_contexts"))
        {
            // each auth context we listen to
            auth_ctx_names.push_back(auth_contexts_node.second.get<std::string>("name"));
        }

        for (auto &action_node : cfg_doorman.get_child("actions"))
        {
            // every action we take
            boost::property_tree::ptree cfg_action = action_node.second;
            DoormanAction a;
            std::string on_status;

            on_status = cfg_action.get<std::string>("on");
            a.on_ = (on_status == "GRANTED" ? AccessStatus::GRANTED : AccessStatus::DENIED);
            a.target_ = cfg_action.get<std::string>("target");
            for (auto &cmd_node : cfg_action.get_child("cmd"))
            {
                // each frame in command
                //fixme ORDER
                //fixme ONLY ONE FRAME
                a.cmd_.push_back(cmd_node.second.data());
            }
            actions.push_back(a);
        }

        INFO("Creating Doorman instance " << doorman_name);
        doormen_.push_back(new DoormanInstance(ctx_,
                doorman_name, auth_ctx_names, actions, timeout));
    }
}
