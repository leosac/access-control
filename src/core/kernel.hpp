#pragma once

#include <boost/property_tree/ptree.hpp>
#include <zmqpp/context.hpp>
#include "tools/runtimeoptions.hpp"
#include "core/netconfig/networkconfig.hpp"
#include "zmodule_manager.hpp"
#include "MessageBus.hpp"

namespace Leosac
{
    /**
    * Core of Leosac. Handles module management and loading.
    * The kernel binds a REP socket at "inproc://leosac-kernel".
    *
    * ### Supported commands
    * Leosac kernel support very few commands. Those commands can be send by modules.
    *
    * Command (Frame 1)        | Frame 2             | Frame 3          | Description
    * -------------------------|---------------------|------------------|--------------
    * RESTART                  |                     |                  | Restart Leosac. This will destroy the kernel object, unload all module, and restart.
    * RESET                    |                     |                  | Reset Leosac configuration to factory default. This will also restart.
    * GET_NETCONFIG            |                     |                  | Send the network config.
    * SET_NETCONFIG            | Serialized ptree    |                  | Write the new network config to file
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
        * Watch for message on the `control_` socket.
        */
        zmqpp::reactor reactor_;

        /**
        * Global configuration.
        */
        boost::property_tree::ptree config_;

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

        enum class EnvironVar
        {
            FACTORY_CONFIG_DIR,
            SCRIPTS_DIR,
        };

        std::map<EnvironVar, std::string> environ_;
    };
}
