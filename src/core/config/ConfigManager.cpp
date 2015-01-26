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
        modules_config.add_child("module", get_module_config(module_name));
    }
    app_config.get_child("kernel").add_child("modules", modules_config);
    return app_config;
}

boost::property_tree::ptree ConfigManager::get_general_config()
{
    boost::property_tree::ptree general_cfg;

    for (const std::string &cfg_name : {"remote", "plugin_directories", "log", "network", "autosave"})
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
    sock_ptr->send(zmqpp::message() << "DUMP_CONFIG" << ConfigFormat::BOOST_ARCHIVE);

    std::string tmp;
    sock_ptr->receive(tmp);

    std::istringstream iss(tmp);
    boost::archive::text_iarchive archive(iss);
    boost::property_tree::ptree cfg;
    boost::property_tree::load(archive, cfg, 1);

    modules_configs_[module] = cfg;
    return cfg;
}

bool ConfigManager::store_config(std::string const &module,
        boost::property_tree::ptree const &cfg)
{
    bool ret;

    if (modules_configs_.count(module))
        ret = true;
    else
        ret = false;

    modules_configs_[module] = cfg;

    return ret;
}

const boost::property_tree::ptree &ConfigManager::load_config(const std::string &module) const
{
    assert(modules_configs_.count(module));
    auto itr = modules_configs_.find(module);
    if (itr != modules_configs_.end())
        return itr->second;
    throw std::runtime_error("");
}

zmqpp::message &operator>>(zmqpp::message &msg, Leosac::ConfigManager::ConfigFormat &fmt)
{
    static_assert(std::is_same<std::underlying_type<Leosac::ConfigManager::ConfigFormat>::type, uint8_t>::value,
            "Bad underlying type for enum");
    uint8_t tmp;
    msg >> tmp;
    fmt = static_cast<Leosac::ConfigManager::ConfigFormat>(tmp);
    return msg;
}

zmqpp::message &operator<<(zmqpp::message &msg, const Leosac::ConfigManager::ConfigFormat &fmt)
{
    static_assert(std::is_same<std::underlying_type<Leosac::ConfigManager::ConfigFormat>::type, uint8_t>::value,
            "Bad underlying type for enum");
    msg << static_cast<uint8_t>(fmt);
    return msg;
}
