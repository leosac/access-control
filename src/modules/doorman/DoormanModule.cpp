#include "DoormanModule.hpp"
#include "DoormanInstance.hpp"
#include "tools/log.hpp"

DoormanModule::DoormanModule(zmqpp::context &ctx,
        zmqpp::socket *pipe,
        const boost::property_tree::ptree &cfg) :
        ctx_(ctx),
        pipe_(*pipe),
        config_(cfg),
        is_running_(true)
{
    process_config();

    for (auto doorman : doormen_)
    {
        reactor_.add(doorman->bus_sub(),
                std::bind(&DoormanInstance::handle_bus_msg, doorman));
    }
    reactor_.add(pipe_, std::bind(&DoormanModule::handle_pipe, this));
}

DoormanModule::~DoormanModule()
{
    for (auto doorman : doormen_)
    {
        delete doorman;
    }
}

void DoormanModule::handle_pipe()
{
    zmqpp::signal s;

    pipe_.receive(s, true);
    if (s == zmqpp::signal::stop)
        is_running_ = false;
}

void DoormanModule::run()
{
    while (is_running_)
    {
        reactor_.poll(-1);
    }
}

void DoormanModule::process_config()
{
    boost::property_tree::ptree module_config = config_.get_child("module_config");

    for (auto &node : module_config.get_child("instances"))
    {
        boost::property_tree::ptree auth_instance_cfg = node.second;

        std::string auth_ctx_name = auth_instance_cfg.get_child("name").data();
        std::string auth_target_name = auth_instance_cfg.get_child("auth_source").data();
        std::string auth_input_file = auth_instance_cfg.get_child("valid_input_file").data();

        LOG() << "Creating Auth instance " << auth_ctx_name;
        doormen_.push_back(new AuthFileInstance(ctx_,
                auth_ctx_name,
                auth_target_name,
                auth_input_file));
    }
}
