#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/property_tree/ptree_serialization.hpp>
#include "ConfigManager.hpp"
#include "core/kernel.hpp"
#include "tools/log.hpp"

using namespace Leosac;

ConfigManager::ConfigManager(Kernel &k) :
        kernel_(k)
{

}

boost::property_tree::ptree ConfigManager::get_application_config()
{
    update_modules_map();
    boost::property_tree::ptree app_config;
    boost::property_tree::ptree modules_config;

    app_config.add_child("kernel", get_general_config());

    std::string module_name;
    std::shared_ptr<zmqpp::socket> sock;
    for (auto &it : modules_sockets_)
    {
        std::tie(module_name, sock) = it;
        if (module_name == "STDIN_CONTROLLER") //fix me : this doesn't implement control socket.
            continue;
        modules_config.add_child("module", get_module_config(module_name));
    }
    app_config.get_child("kernel").add_child("modules", modules_config);
    return app_config;
}

boost::property_tree::ptree ConfigManager::get_general_config()
{
    boost::property_tree::ptree general_cfg;

    for (const std::string &cfg_name : {"remote", "plugin_directories", "log", "network"})
    {
        auto child_opt = kernel_.get_config().get_child_optional(cfg_name);
        if (child_opt)
            general_cfg.add_child(cfg_name, *child_opt);
    }
    return general_cfg;
}

void ConfigManager::update_modules_map()
{
    std::vector<std::string> modules = kernel_.module_manager().modules_names();

    // create socket for modules.
    for (const std::string &module : modules)
    {
        if (modules_sockets_.count(module))
        {
            // we disconnect-reconnect. This is inproc so its fast. It prevents trouble when a module
            // has been reloaded.
            modules_sockets_[module]->disconnect("inproc://module-" + module);
            modules_sockets_[module]->connect("inproc://module-" + module);
        }
        else
        {
            modules_sockets_[module] = std::make_shared<zmqpp::socket>(kernel_.get_context(), zmqpp::socket_type::req);
            modules_sockets_[module]->connect("inproc://module-" + module);
        }
    }
}

boost::property_tree::ptree ConfigManager::get_module_config(std::string const &module)
{
    auto sock_ptr = modules_sockets_[module];
    assert(sock_ptr);

    // get config as a serialized ptree.
    sock_ptr->send(zmqpp::message() << "DUMP_CONFIG" << uint8_t('0'));

    std::string         tmp;
    std::istringstream  iss(tmp);
    sock_ptr->receive(tmp);

    boost::archive::text_iarchive   archive(iss);
    boost::property_tree::ptree     cfg;
    boost::property_tree::load(archive, cfg, 1);
    return cfg;
}
