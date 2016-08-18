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
#include <tools/XmlPropertyTree.hpp>
#include <tools/PropertyTreeExtractor.hpp>
#include "ConfigManager.hpp"
#include "core/kernel.hpp"
#include "tools/log.hpp"

using namespace Leosac;

ConfigManager::ConfigManager(const boost::property_tree::ptree &cfg)
    : kernel_config_(cfg)
    , version_(0)
{
    Tools::PropertyTreeExtractor extractor(cfg, "General Config");

    version_       = extractor.get<uint64_t>("version", 0);
    instance_name_ = extractor.get<std::string>("instance_name");
}

boost::property_tree::ptree ConfigManager::get_application_config()
{
    boost::property_tree::ptree app_config;
    boost::property_tree::ptree modules_config;

    app_config.add_child("kernel", get_general_config());

    std::string module_name;
    boost::property_tree::ptree tree;
    for (auto &it : modules_configs_)
    {
        std::tie(module_name, tree) = it;
        modules_config.add_child("module", load_config(module_name));
    }
    app_config.get_child("kernel").add_child("modules", modules_config);
    return app_config;
}

boost::property_tree::ptree ConfigManager::get_general_config() const
{
    boost::property_tree::ptree general_cfg;

    for (const std::string &cfg_name :
         {"remote", "plugin_directories", "log", "network", "autosave", "sync_dest",
          "no_import", "instance_name"})
    {
        auto child_opt = kernel_config_.get_child_optional(cfg_name);
        if (child_opt)
            general_cfg.add_child(cfg_name, *child_opt);
    }
    // Use the in-memory configuration version.
    general_cfg.add("version", config_version());
    return general_cfg;
}

boost::property_tree::ptree ConfigManager::get_exportable_general_config() const
{
    auto general_cfg = get_general_config();
    auto child_opt   = kernel_config_.get_child_optional("sync_source");
    std::vector<std::string> exportable_config;
    boost::property_tree::ptree ret;

    if (!child_opt)
    {
        // return all minus the `no_import` tag.
        general_cfg.erase("no_import");
        general_cfg.erase("instance_name");
        return general_cfg;
    }

    for (const auto &c : *child_opt)
    {
        ASSERT_LOG(c.first != "no_import" || c.first == "instance_name",
                   "Cannot export the " << c.first
                                        << " tag. Check your"
                                           " `sync_source` configuration tag.");
        if (c.first == "no_import" || c.first == "instance_name")
        {
            WARN("You cannot export the " << c.first << " tag.");
            continue;
        }
        if (c.second.get_value<bool>())
            exportable_config.push_back(c.first);
    }

    for (const auto &child : general_cfg)
    {
        if (std::find(exportable_config.begin(), exportable_config.end(),
                      child.first) != exportable_config.end())
        {
            ret.add_child(child.first, child.second);
        }
    }
    return ret;
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

const boost::property_tree::ptree &
ConfigManager::load_config(const std::string &module) const
{
    assert(modules_configs_.count(module));
    auto itr = modules_configs_.find(module);
    if (itr != modules_configs_.end())
        return itr->second;
    throw std::runtime_error("");
}

zmqpp::message &operator>>(zmqpp::message &msg,
                           Leosac::ConfigManager::ConfigFormat &fmt)
{
    static_assert(
        std::is_same<std::underlying_type<Leosac::ConfigManager::ConfigFormat>::type,
                     uint8_t>::value,
        "Bad underlying type for enum");
    uint8_t tmp;
    msg >> tmp;
    fmt = static_cast<Leosac::ConfigManager::ConfigFormat>(tmp);
    return msg;
}

zmqpp::message &operator<<(zmqpp::message &msg,
                           const Leosac::ConfigManager::ConfigFormat &fmt)
{
    static_assert(
        std::is_same<std::underlying_type<Leosac::ConfigManager::ConfigFormat>::type,
                     uint8_t>::value,
        "Bad underlying type for enum");
    msg << static_cast<uint8_t>(fmt);
    return msg;
}

const boost::property_tree::ptree &ConfigManager::kconfig() const
{
    return kernel_config_;
}

boost::property_tree::ptree &ConfigManager::kconfig()
{
    return kernel_config_;
}

bool ConfigManager::remove_config(const std::string &module)
{
    if (modules_configs_.find(module) != modules_configs_.end())
    {
        modules_configs_.erase(module);
        return true;
    }
    return false;
}

void ConfigManager::set_kconfig(boost::property_tree::ptree const &new_cfg)
{
    INFO("Attempting to set kernel config. We need to somehow merge.");
    auto kernel_cfg_file = kernel_config_.get<std::string>("kernel-cfg");

    auto child_opt = kernel_config_.get_child_optional("sync_dest");
    std::vector<std::string> exportable_config;
    boost::property_tree::ptree ret;

    if (!child_opt)
    {
        auto no_import_child = kernel_config_.get_child_optional("no_import");
        boost::property_tree::ptree cpy;
        if (no_import_child)
        {
            cpy = *no_import_child;
        }
        // import all (except no_import tag)
        kernel_config_ = new_cfg;
        // restore
        if (no_import_child)
            kernel_config_.put_child("no_import", cpy);
        kernel_config_.add("kernel-cfg", kernel_cfg_file);
        kernel_config_.put("instance_name", instance_name());
    }
    else
    {
        for (const auto &c : *child_opt)
        {
            if (c.second.get_value<bool>())
                exportable_config.push_back(c.first);
        }

        for (const auto &child : new_cfg)
        {
            if (std::find(exportable_config.begin(), exportable_config.end(),
                          child.first) != exportable_config.end())
            {
                kernel_config_.put_child(child.first, child.second);
            }
        }
    }
}

std::list<std::string> ConfigManager::get_non_importable_modules() const
{
    std::list<std::string> ret;
    const auto &no_import = kernel_config_.get_child_optional("no_import");

    if (no_import)
    {
        for (const auto &module : *no_import)
        {
            ret.push_back(module.first);
        }
    }
    return ret;
}

bool ConfigManager::is_module_importable(std::string const &module_name) const
{
    auto lst = get_non_importable_modules();
    return std::find(lst.begin(), lst.end(), module_name) == lst.end();
}

uint64_t ConfigManager::config_version() const
{
    return version_;
}

void ConfigManager::incr_version()
{
    ++version_;
}

bool ConfigManager::has_config(const std::string &module) const
{
    return modules_configs_.count(module);
}

void ConfigManager::config_version(uint64_t new_version)
{
    version_ = new_version;
}

const std::string &ConfigManager::instance_name() const
{
    return instance_name_;
}
