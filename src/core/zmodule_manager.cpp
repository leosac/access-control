#include <algorithm>
#include <tools/unixfs.hpp>
#include <tools/log.hpp>
#include <exception/dynlibexception.hpp>
#include <exception/moduleexception.hpp>
#include "zmodule_manager.hpp"
#include <boost/property_tree/ptree.hpp>
#include <zmqpp/context.hpp>

void zModuleManager::unloadLibraries()
{
    for (auto &module_info : BLABLA_)
    {
        try
        {
            module_info.lib_->close();
        }
        catch (const DynLibException &e)
        {
            throw (ModuleException(e.what()));
        }
        //delete module_info.lib_;
        // module_info.lib_ = nullptr;
    }
    BLABLA_.clear();
}


bool zModuleManager::initModules()
{
    for (auto &module_info : BLABLA_)
    {
        try
        {
            void *symptr = module_info.lib_->getSymbol("start_module");
            assert(symptr);
            std::function<bool(zmqpp::socket * )> actor_fun = std::bind(
                    ((bool (*)(zmqpp::socket *, boost::property_tree::ptree, zmqpp::context &)) symptr),
                    std::placeholders::_1, // placeholder for pipe
                    module_info.config_,
                    std::ref(ctx_));
            zmqpp::actor new_module(actor_fun);
            modules_.push_back(std::move(new_module));

            LOG() << "Module {" << module_info.name_ << "} initialized. (level = " <<
                    module_info.config_.get<int>("level", 100);
        }
        catch (std::exception &e)
        {
            LOG() << "Unable to init module {" << module_info.name_ << "}: " << e.what();
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
            ModuleInfo module_info;

            module_info.name_ = module_name;
            module_info.config_ = cfg;
            if (!(module_info.lib_ = load_library_file(path_entry + "/" + filename)))
                return false;
            BLABLA_.insert(module_info);
            LOG() << "library file loaded (not init yet)";
            return true;
        }

    }
    LOG() << "Could'nt load this module";
    return false;
}

DynamicLibrary *zModuleManager::load_library_file(const std::string &full_path)
{
    DynamicLibrary *lib = nullptr;

    LOG() << "Loading library at: " << full_path;
    lib = new DynamicLibrary(full_path);
    try
    {
        lib->open(DynamicLibrary::RelocationMode::Now);
    }
    catch (const DynLibException &e)
    {
        delete lib;
        LOG() << "FAILURE, full path was:{" << full_path << "}";
        return nullptr;
    }
    return lib;
}

void zModuleManager::stopModules()
{
    std::reverse(modules_.begin(), modules_.end());
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

zModuleManager::zModuleManager(zmqpp::context &ctx) :
        ctx_(ctx)
{

}
