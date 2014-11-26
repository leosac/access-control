#include "AuthFileModule.hpp"
#include "tools/log.hpp"

using namespace Leosac::Module::Auth;

AuthFileModule::AuthFileModule(zmqpp::context &ctx,
        zmqpp::socket *pipe,
        const boost::property_tree::ptree &cfg) :
        BaseModule(ctx, pipe, cfg)
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
        boost::property_tree::ptree auth_instance_cfg = node.second;

        std::string auth_ctx_name = auth_instance_cfg.get_child("name").data();
        std::string auth_source_name = auth_instance_cfg.get_child("auth_source").data();
        std::string config_file = auth_instance_cfg.get_child("config_file").data();
        std::string auth_target_name = auth_instance_cfg.get<std::string>("target", "");

        INFO("Creating Auth instance " << auth_ctx_name);
        authenticators_.push_back(AuthFileInstancePtr(new AuthFileInstance(ctx_,
                auth_ctx_name,
                auth_source_name,
                auth_target_name,
                config_file)));
    }
}
