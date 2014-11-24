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

            on_status = cfg_action.get<std::string>("on") == "GRANTED";
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
