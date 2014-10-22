#pragma once

#include <boost/property_tree/ptree.hpp>
#include <tools/runtimeoptions.hpp>
#include <zmqpp/context.hpp>
#include <core/netconfig/networkconfig.hpp>
#include "zmodule_manager.hpp"
#include "MessageBus.hpp"



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

    ~Kernel();

    /**
    * Disable copy constructor as it makes no sense to copy this.
    */
    Kernel(const Kernel &) = delete;

    /**
    * Disable assignment operator.
    */
    Kernel &operator=(const Kernel &) = delete;

    /**
    * Build a property tree from a runtime object object.
    * It assume the kernel-config (-k) switch points to an XML config file.
    * If we support more config source type (json) we should move this code.
    */
    static boost::property_tree::ptree make_config(const RuntimeOptions &opt);

    /**
    * Main loop of the main thread.
    * What it does:
    *      1. It starts the communication BUS for devices's abstraction and general module to use.
    *      2. It starts by loading modules requested in configuration
    *      3. Wait for shutdown / restart order (linux signals).
    * @return If it returns true that means the application should restart instead of stopping.
    */
    bool run();

private:

    /**
    * Init the module manager by feeding it paths to library file, loading module, etc.
    */
    bool module_manager_init();

    /**
    * A request has arrived on the `control_` socket.
    */
    void handle_control_request();

    /**
    * Reset Leosac configuration.
    */
    void factory_reset();

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
    NetworkConfig network_config_;

    /**
    * Hardcoded path to factory default settings.
    */
    static constexpr const char *rel_path_to_factory_conf_ = "./cfg/factory";
};