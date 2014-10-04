#pragma once
#include <list>
#include <map>
#include <string>
#include <vector>
#include <dynlib/dynamiclibrary.hpp>
#include <modules/moduleconfig.h>
#include <boost/property_tree/ptree_fwd.hpp>

#include "zmqpp/actor.hpp"

/**
* A second module manager that loads "ZMQ aware" module -- modules that talks to the application through message passing.
* A second loader is necessary because the procedure of loading a module differs.
* No reference to an object is returned when loading a module. The module will lives in its own thread and other components
* can talk to it.
*
* @note: Only the zModuleManager object has "direct access" to the module object. It is zmqpp::actor launch the module
* in its own thread.
*/
class zModuleManager
    {
public:

    ~zModuleManager();

    void        unloadLibraries();

    /**
    * Actually call the init_module() function of each library we loaded.
    * @throws: may throw ModuleException if init_module() fails for a library (or actor init exception).
    */
    bool        initModules();

    /**
    * Opposite of init module. this stop all modules thread and perform cleanup.
    * To dynamic library handlers are NOT released.
    */
    void        stopModules();

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
    bool loadModule(const boost::property_tree::ptree &cfg);

private:
    /**
    * This will load (actually calling dlopen()) the library file located at full_path.
    */
    void    load_library_file(const std::string &full_path);

private:
    std::vector<std::string> path_;
    std::map<std::string, DynamicLibrary*>      _dynlibs;

    /**
    * Since module live is their own thread, we use the actor pattern.
    */
    std::vector<zmqpp::actor> modules_;
};