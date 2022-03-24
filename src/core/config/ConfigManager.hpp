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

#pragma once

#include "exception/leosacexception.hpp"
#include "tools/log.hpp"
#include <boost/property_tree/ptree.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <zmqpp/zmqpp.hpp>

namespace Leosac
{
class Kernel;

/**
* That class helps manage the configuration for the application and its module.
* It's goal is to help saving the configuration to be used to maintain each module
* configuration.
*
* The `load_config()` and `store_config()` are used to retrieve or update module
* config.
* For example, when remote control is doing a SYNC_FROM, it pushes the new config
* to the ConfigManager using `store_config()`.
*/
class ConfigManager
{
  public:
    /**
     * Construct the Configuration Manager from a property tree.
     */
    ConfigManager(const boost::property_tree::ptree &cfg);
    virtual ~ConfigManager() = default;

    ConfigManager(const ConfigManager &) = default;

    ConfigManager(ConfigManager &&) = delete;
    ConfigManager &operator=(const ConfigManager &) = delete;
    ConfigManager &operator=(ConfigManager &&) = delete;

    /**
    * Extract the current "general configuration".
    *
    * General configuration means anything that is not module configuration,
    * i/e "log", "network" and "remote".
    *
    * @note This is part of the process to build the whole up-to-date config tree.
    */
    boost::property_tree::ptree get_general_config() const;

    /**
    * Return the property_tree of item inside the `<kernel>` tag (except <modules>`)
    * that are marked exportable.
    *
    * See the `<sync_source>` tag in @ref general_config_main.
    */
    boost::property_tree::ptree get_exportable_general_config() const;

    /**
    * Retrieve the (current, running) configuration from the application and
    * its modules.
    *
    * Since the ConfigManager shall have an up-to-date view of the modules'
    * configuration
    * tree, it can build a global config tree.
    * It returns a proper property tree that can be serialized.
    */
    boost::property_tree::ptree get_application_config();

    /**
    * Store the configuration tree for a module.
    * If pushing the configuration overwrote a previously existing one, this will
    * return
    * true. Otherwise it will return false.
    */
    bool store_config(const std::string &module,
                      const boost::property_tree::ptree &cfg);

    /**
     * Do we have config information for the module.
     */
    bool has_config(const std::string &module) const;

    /**
    * Return the stored configuration for a given module.
    */
    const boost::property_tree::ptree &load_config(const std::string &module) const;

    /**
    * Remove the config entry for the module named module.
    *
    * Return true if successfully removed entry, false otherwise.
    */
    bool remove_config(const std::string &module);

    /**
    * This enum is used internally, when core request module configuration.
    */
    enum class ConfigFormat : uint8_t
    {
        BOOST_ARCHIVE = 0,
        XML           = 1,
    };

    /**
    * Return const & on the general config tree.
    */
    const boost::property_tree::ptree &kconfig() const;

    /**
    * Returns non-const ref to general config tree.
    */
    boost::property_tree::ptree &kconfig();

    /**
    * Update Leosac's core config ptree.
    *
    * This function filter the new_cfg tree and does not store
    * configuration for category that are excluded through the `sync_dest` option.
    */
    void set_kconfig(const boost::property_tree::ptree &new_cfg);

    /**
    * Returns a list of module name that should not be imported.
    * Imported means added or updated when a SYNC_FROM is executed.
    */
    std::list<std::string> get_non_importable_modules() const;

    /**
    * Helper around get_non_importable_modules()
    */
    bool is_module_importable(const std::string &) const;

    /**
     * Return the current configuration version.
     * This is supposed to work similar to Bind9 serial.
     *
     * It should only ever increase.
     */
    uint64_t config_version() const;

    void config_version(uint64_t new_version);

    /**
     * Increment by 1 the current version number.
     */
    void incr_version();

    /**
     * Return the name of the instanced assigned in the configuration file.
     * @note This method is thread safe because the instance name shall never
     * change.
     */
    const std::string &instance_name() const;

  private:
    /**
    * Maps a module's name to a property tree object.
    */
    std::map<std::string, boost::property_tree::ptree> modules_configs_;

    /**
    * Property tree for general configuration.
    * This is the base config tree.
    */
    boost::property_tree::ptree kernel_config_;

    uint64_t version_;

    std::string instance_name_;
};
}

zmqpp::message &operator>>(zmqpp::message &msg,
                           Leosac::ConfigManager::ConfigFormat &fmt);

zmqpp::message &operator<<(zmqpp::message &msg,
                           const Leosac::ConfigManager::ConfigFormat &fmt);
