#include <algorithm>
#include <tools/unixfs.hpp>
#include <tools/log.hpp>
#include <exception/dynlibexception.hpp>
#include <exception/moduleexception.hpp>
#include "zmodule_manager.hpp"
#include <boost/property_tree/ptree.hpp>

void zModuleManager::unloadLibraries()
{
    for (auto& lib : _dynlibs)
    {
        try {
            lib.second->close();
        }
        catch (const DynLibException& e) {
            throw (ModuleException(e.what()));
        }
        delete lib.second;
    }
    _dynlibs.clear();
}


bool zModuleManager::initModules()
    {
    for (auto & dynlib : _dynlibs)
        {
        try
            {
            void *symptr = dynlib.second->getSymbol("start_module");
            assert(symptr);
            zmqpp::actor new_module((bool (*)(zmqpp::socket *)) symptr);

            modules_.push_back(std::move(new_module));
            }
        catch (std::exception &e)
            {
            LOG() << "Unable to init module: " << e.what();
            return false;
            }
        }
    return true;
    }

void zModuleManager::addToPath(const std::string &dir)
    {
    if (std::find(path_.begin(), path_.end(), dir) == path_.end())
        path_.push_back(dir);
    }

bool zModuleManager::loadModule(const boost::property_tree::ptree &cfg)
    {
    std::string filename = cfg.get_child("file").data();
    std::string module_name = cfg.get_child("name").data();

    LOG() << "Attempting to load module nammed " << module_name << " (shared lib file = " << filename << ")";
    for (const std::string &path_entry : path_)
        {
        // fixme not clean enough.
        if (UnixFs::fileExists(path_entry + "/" + filename))
            {
            load_library_file(path_entry + "/" + filename);
            LOG() << "library file loaded (not init yet)";
            return true;
            }

        }
    LOG() << "Could'nt load this module";
    return false;
    }

void zModuleManager::load_library_file(const std::string &full_path)
    {

    std::string     libname(UnixFs::stripPath(full_path));
    DynamicLibrary* lib;

    LOG() << "Loading " << libname << "(full path: " << full_path << ")";
    if (_dynlibs.count(libname) > 0)
        throw (ModuleException("library already loaded at " + _dynlibs.at(libname)->getFilePath()));
    lib = new DynamicLibrary(full_path);
    try {
        lib->open(DynamicLibrary::RelocationMode::Now);
    }
    catch (const DynLibException& e) {
        delete lib;
        throw (ModuleException(e.what()));
    }
    _dynlibs[libname] = lib;
    }

void zModuleManager::stopModules()
    {
    for (zmqpp::actor &a : modules_)
        {
        a.stop(true);
        }
    }

zModuleManager::~zModuleManager()
    {
    stopModules();
    unloadLibraries();
    }
