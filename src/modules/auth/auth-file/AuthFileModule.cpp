#include "AuthFileModule.hpp"
#include "AuthFileInstance.hpp"
#include "tools/log.hpp"

AuthFileModule::AuthFileModule(zmqpp::context &ctx,
        zmqpp::socket *pipe,
        const boost::property_tree::ptree &cfg) :
        ctx_(ctx),
        pipe_(*pipe),
        config_(cfg),
        is_running_(true)
{
    process_config();

    for (auto authenticator : authenticators_)
    {
        reactor_.add(authenticator->bus_sub(),
                std::bind(&AuthFileInstance::handle_bus_msg, authenticator));
    }
    reactor_.add(pipe_, std::bind(&AuthFileModule::handle_pipe, this));
}

AuthFileModule::~AuthFileModule()
{
    for (auto authenticator : authenticators_)
    {
        delete authenticator;
    }
}

void AuthFileModule::handle_pipe()
{
    zmqpp::signal s;

    pipe_.receive(s, true);
    if (s == zmqpp::signal::stop)
        is_running_ = false;
}

void AuthFileModule::run()
{
    while (is_running_)
    {
        reactor_.poll(-1);
    }
}

void AuthFileModule::process_config()
{
    boost::property_tree::ptree module_config = config_.get_child("module_config");

    for (auto &node : module_config.get_child("instances"))
    {
        boost::property_tree::ptree auth_instance_cfg = node.second;

        std::string auth_ctx_name = auth_instance_cfg.get_child("name").data();
        std::string auth_target_name = auth_instance_cfg.get_child("auth_source").data();
        std::string auth_input_file = auth_instance_cfg.get_child("valid_input_file").data();

        LOG() << "Creating Auth instance " << auth_ctx_name;
        authenticators_.push_back(new AuthFileInstance(ctx_,
                auth_ctx_name,
                auth_target_name,
                auth_input_file));
    }
}
