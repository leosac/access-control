#pragma once

#include <boost/property_tree/ptree.hpp>
#include <tools/runtimeoptions.hpp>
#include "zmodule_manager.hpp"

/**
* Core of Leosac. Handles module management and loading.
*/
class Kernel
    {
public:
    Kernel(const boost::property_tree::ptree &config);

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
    * Global configuration.
    */
    boost::property_tree::ptree config_;

    /**
    * Init the module manager by feeding it paths to library file, loading module, etc.
    */
    bool module_manager_init();

    /**
    * Manages the different libraries (.so) we load, path to those libraries, modules instanciation.
    */
    zModuleManager module_manager_;

    /**
    * Controls core main loop.
    */
    bool is_running_;
    };