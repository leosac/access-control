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

#include <boost/property_tree/ptree.hpp>
#include <zmqpp/context.hpp>
#include <core/config/ConfigManager.hpp>
#include "tools/runtimeoptions.hpp"
#include "core/netconfig/networkconfig.hpp"
#include "zmodule_manager.hpp"
#include "MessageBus.hpp"
#include "RemoteControl.hpp"

namespace Leosac
{
    /**
    * Core of Leosac. Handles module management and loading.
    * The kernel binds a REP socket at "inproc://leosac-kernel".
    *
    * ### Supported commands
    * Leosac kernel support a few commands. Those commands can be send by modules.
    *
    * Command (Frame 1)        | Frame 2             | Frame 3          | Description
    * -------------------------|---------------------|------------------|--------------
    * RESTART                  |                     |                  | Restart Leosac. This will destroy the kernel object, unload all module, and restart.
    * RESET                    |                     |                  | Reset Leosac configuration to factory default. This will also restart.
    * GET_NETCONFIG            |                     |                  | Send the network config.
    * SET_NETCONFIG            | Serialized ptree    |                  | Write the new network config to file
    * SCRIPTS_DIR              |                     |                  | Ask the path to scripts directory
    * FACTORY_CONFIG_DIR       |                     |                  | Ask the path to factory config directory
    *
    * @note Leosac kernel will publish on the message bus a `SYSTEM_READY` message when all modules are
    * initialized. This message's topic is "KERNEL".
    */
    class Kernel
    {
    public:
        /**
        * Construct a Kernel object. Only one should live, it doesn't make sense to have more than one, and could
        * cause problem.
        *
        * @param config initial configuration tree
        * @note You can use Kernel::make_config() to build a configuration tree.
        */
        explicit Kernel(const boost::property_tree::ptree &config);

        /**
        * Disable copy constructor as it makes no sense to copy this.
        */
        Kernel(const Kernel &) = delete;

        /**
        * Disable assignment operator.
        */
        Kernel &operator=(const Kernel &) = delete;

        /**
        * Disable move-assignment.
        */
        Kernel &operator=(Kernel &&) = delete;

        /**
        * Disable move-construction.
        */
        Kernel(Kernel &&) = delete;

        /**
        * Build a property tree from a runtime object object.
        * It assume the kernel-config (-k) switch points to an XML config file.
        * If we support more config source type (json) we should move this code.
        */
        static boost::property_tree::ptree make_config(const Leosac::Tools::RuntimeOptions &opt);

        /**
        * Main loop of the main thread.
        * What it does:
        *      1. It starts the communication BUS for devices's abstraction and general module to use.
        *      2. It starts by loading modules requested in configuration
        *      3. Wait for shutdown / restart order (linux signals).
        * @return If it returns true that means the application should restart instead of stopping.
        */
        bool run();

        /**
        * Return the path to the scripts directory.
        * Uses environment variable if available, otherwise defaults to hardcoded path relative to cwd.
        */
        std::string script_directory() const;

        /**
        * Return the path to factory config directory
        * Uses environment variable if available, otherwise defaults to hardcoded path relative to cwd.
        */
        std::string factory_config_directory() const;

        const zModuleManager &module_manager() const;
        zModuleManager &module_manager();

        /**
        * Returns a reference to the zmqpp context create for the application.
        */
        zmqpp::context &get_context();

        ConfigManager &config_manager();

        /**
        * Save the current configuration to its original file if `autosave` is enabled.
        * This means that configuration change made when Leosac was running will be persisted.
        */
        bool save_config();

    private:
        /**
        * Init the module manager by feeding it paths to library file, loading module, etc.
        */
        void module_manager_init();

        /**
        * A request has arrived on the `control_` socket.
        */
        void handle_control_request();

        /**
        * Reset Leosac configuration.
        */
        void factory_reset();

        /**
        * Handle GET_NETCONFIG command.
        */
        void get_netconfig();

        /**
        * Handle SET_NETCONFIG command and update the configuration file directly.
        * The configuration update will take effect on the next restart.
        * @param msg ZMQ message that holds config
        */
        void set_netconfig(zmqpp::message *msg);

        void extract_environ();

        void configure_logger();

        /**
        * Query all modules and retrieve the full application configuration.
        */
        std::string get_full_config();

        ConfigManager config_manager_;

        /**
        * The application ZMQ context.
        */
        zmqpp::context ctx_;

        /**
        * Application wide (inproc) message bus.
        */
        MessageBus bus_;

        /**
        * A REP socket to send request to the kernel.
        */
        zmqpp::socket control_;

        /**
        * A PUSH socket to write on the bus.
        */
        zmqpp::socket bus_push_;

        /**
        * Watch for message on the `control_` socket.
        */
        zmqpp::reactor reactor_;

        /**
        * Controls core main loop.
        */
        bool is_running_;

        /**
        * Should leosac restart ?
        */
        bool want_restart_;

        /**
        * Manages the different libraries (.so) we load, path to those libraries, modules instantiation.
        */
        zModuleManager module_manager_;

        /**
        * Object that handle networking configuration.
        */
        std::unique_ptr<NetworkConfig> network_config_;

        /**
        * Object that expose leosac to the world.
        */
        std::unique_ptr<RemoteControl> remote_controller_;

        enum class EnvironVar
        {
            FACTORY_CONFIG_DIR,
            SCRIPTS_DIR,
        };

        std::map<EnvironVar, std::string> environ_;

        /**
        * Autosave configuration on shutdown.
        */
        bool autosave_;

    };
}
