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

#pragma once

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
    * It's goal is to help saving the configuration and to extract the runtime configuration
    * from each module.
    */
    class ConfigManager
    {
    public:
        ConfigManager(Kernel &k);
        virtual ~ConfigManager() = default;
        
        ConfigManager(const ConfigManager &) = delete;
        ConfigManager(ConfigManager &&) = delete;
        ConfigManager &operator=(const ConfigManager &) = delete;
        ConfigManager &operator=(ConfigManager &&) = delete;

        /**
        * Retrieve the (current, running) configuration from the application and
        * its modules.
        *
        * This is done by asking every module to dump their current configuration.
        * It returns a proper property tree that can be serialized.
        */
        boost::property_tree::ptree get_application_config();

    private:
        /**
        * Extract the current "general configuration".
        *
        * General configuration means anything that is not module configuration,
        * i/e "log", "network" and "remote".
        *
        * @note This is part of the process to build the whole up-to-date config tree.
        */
        boost::property_tree::ptree get_general_config();

        /**
        * Retrieve configuration option for a module.
        */
        boost::property_tree::ptree get_module_config(const std::string &module);

        /**
        * Use the module manager to get an up-to-date list of running modules
        * and adapts its module_sockets_ map accordingly.
        */
        void update_modules_map();

        Kernel &kernel_;

        /**
        * Maps a socket to a module (by using the module's name).
        */
        std::map<std::string, std::shared_ptr<zmqpp::socket>> modules_sockets_;
    };
}
