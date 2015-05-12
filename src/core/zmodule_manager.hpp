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

#include <list>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <zmqpp/actor.hpp>
#include "dynlib/dynamiclibrary.hpp"
#include "boost/property_tree/ptree.hpp"
#include "core/config/ConfigManager.hpp"
#include "LeosacFwd.hpp"

namespace Leosac
{
    class ConfigManager;
}

/**
* A second module manager that loads "ZMQ aware" module -- modules that talks to the application through message passing.
* A second loader is necessary because the procedure of loading a module differs.
* No reference to an object is returned when loading a module. The module will lives in its own thread and other components
* can talk to it.
*
* @note: Only the zModuleManager object has "direct access" to the module object. It is zmqpp::actor that launches the module
* in its own thread.
*
* @note: Use the "level" property to define module initialization order.
* This initialization order is mandatory, and the lower the value is, the sooner the module is loaded.
*/
class zModuleManager
{
public:
    /**
    * Construct the module manager.
    * @param ctx the zeroMQ context to pass around to module.
    */
    zModuleManager(zmqpp::context &ctx, Leosac::Kernel &k);

    ~zModuleManager();


    /**
    * Internal helper struct that store informations related to module
    * that are useful to the module manager.
    *
    * @note This goes into a Set, but we need non-const access to the
    * actor_ pointer.
    */
    struct ModuleInfo
    {
        ~ModuleInfo();
        ModuleInfo(const Leosac::ConfigManager &cfg);

        ModuleInfo(const ModuleInfo &) = delete;
        ModuleInfo &operator=(const ModuleInfo &) = delete;

        /**
        * Enable move constructor for std::set
        */
        ModuleInfo(ModuleInfo &&o);

        ModuleInfo &operator=(ModuleInfo &&) = delete;

        /**
        * Name of the module, as specified in the configuration file.
        */
        std::string name_;

        /**
        * Pointer to the library object.
        */
        mutable std::shared_ptr<DynamicLibrary> lib_;

        /**
        * Actor object that runs the module code.
        */
        mutable std::unique_ptr<zmqpp::actor> actor_;

        /**
        * Comparison operator: used by the module manager set.
        * We do not really care about uniqueness, we use this
        * to sort module by their loading priority.
        */
        bool operator<(const ModuleInfo &o) const
        {
            int level_me = cfg_.load_config(name_).get<int>("level", 100);
            int level_o = cfg_.load_config(o.name_).get<int>("level", 100);

            return level_me < level_o;
        }

    private:
        const Leosac::ConfigManager &cfg_;
    };

    /**
    * Actually call the init_module() function of each library we loaded.
    * The module initialization order is honored because the modules_ set is ordered.
    * @throws: may throw ModuleException if init_module() fails for a library (or actor init exception).
    */
    void initModules();

    /**
    * Attempt to find a module using its name, then load it.
    * @return false if a module with this name cannot be found. True otherwise.
    * @note Even if the module is found, loading it could fail. In that case, an exception would be thrown.
    */
    bool initModule(const std::string &name);

    /**
    * Initialize a module.
    */
    void initModule(ModuleInfo *modinfo);

    /**
    * Opposite of init module. this stop all modules thread and perform cleanup.
    * @note Dynamic libraries handlers are NOT released.
    */
    void stopModules();

    /**
    * Stop a module by name and remove its config info from the config manager.
    * @note This does not unload the underlying shared library.
    * @note Stopping individual module must be done carefully. Be careful wrt dependencies between modules.
    *       It's recommended to also stop modules that depends on the module you initially wanted to stop.
    * @return false if a module with this name cannot be found. True otherwise.
    */
    bool stopModule(const std::string &name);

    /**
    * Stop a specific module and remove its config info from the config manager.
    * @note This does not unload the underlying shared library.
    * @note Stopping individual module must be done carefully. Be careful wrt dependencies between modules.
    *       It's recommended to also stop modules that depends on the module you initially wanted to stop.
    * @return false if a module with this name cannot be found. True otherwise.
    */
    void stopModule(ModuleInfo *modinfo);

    /**
    * Add a directory to a path. If the path already exist, it is ignored.
    */
    void addToPath(const std::string &dir);

    /**
    * Search the path and load a module based on a property tree for this module.
    * Mandatory data are the module "name" and the "file" location (the name of the .so to load).
    *
    * The first file to match (looping over the path array) will be loaded.
    */
    bool loadModule(const std::string &module_name);

    /**
    * Returns the list of the name of the loaded modules.
    */
    std::vector<std::string> modules_names() const;

    /**
    * Do we have some informations about the module "name".
    *
    * It returns true if we have it in our modules_ list. Returning true here means
    * that we have a shared library handler, thus meaning the module is loaded (maybe not
    * initialized).
    */
    bool has_module(const std::string &name) const;

    /**
    * Return the list of paths where we search for module.
    */
    const std::vector<std::string> &get_module_path() const;

private:
    /**
    * Close library handler.
    *
    * Cleanup code, call from destructor only.
    */
    void unloadLibraries();

    ModuleInfo *find_module_by_name(const std::string &name) const;

    /**
    * This will load (actually calling dlopen()) the library file located at full_path.
    * It returns a DynamicLibrary pointer that can be used to retrieve symbol from the shared object.
    * It it failed, returns nullptr.
    */
    std::shared_ptr<DynamicLibrary> load_library_file(const std::string &full_path);

    std::vector<std::string> path_;
    std::set<ModuleInfo> modules_;

    zmqpp::context &ctx_;
    Leosac::ConfigManager &config_manager_;
    Leosac::CoreUtilsPtr  core_utils_;
};