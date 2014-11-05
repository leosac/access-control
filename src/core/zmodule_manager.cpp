#include <algorithm>
#include <tools/unixfs.hpp>
#include <tools/log.hpp>
#include <exception/dynlibexception.hpp>
#include <exception/moduleexception.hpp>
#include "zmodule_manager.hpp"
#include <boost/property_tree/ptree.hpp>
#include <zmqpp/zmqpp.hpp>
#include <exception/ExceptionsTools.hpp>

using Leosac::Tools::UnixFs;

void zModuleManager::unloadLibraries()
{
    for (auto &module_info : modules_)
    {
        try
        {
            module_info.lib_->close();
        }
        catch (const DynLibException &e)
        {
            std::throw_with_nested(ModuleException("Unloading library failed."));
        }
    }
    modules_.clear();
}

bool zModuleManager::initModules()
{
    for (const ModuleInfo &module_info : modules_)
    {
        try
        {
            void *symptr = module_info.lib_->getSymbol("start_module");
            assert(symptr);
            // take the module init function and make a std::function out of it.
            std::function<bool(zmqpp::socket *, boost::property_tree::ptree, zmqpp::context &)> actor_fun =
                    ((bool (*)(zmqpp::socket *, boost::property_tree::ptree, zmqpp::context &)) symptr);

            // std::function that is valid for zmqpp::actor. It store ctx, config and the real module init function.
            std::function<bool(zmqpp::socket *)> helper_function = std::bind(
                    &zModuleManager::start_module_helper,
                    std::placeholders::_1, // actor pipe,
                    module_info.config_,
                    std::ref(ctx_),
                    actor_fun);

            zmqpp::actor *new_module = new zmqpp::actor(helper_function);
            module_info.actor_ = new_module;

            INFO("Module {" << module_info.name_ << "} initialized. (level = " <<
                    module_info.config_.get<int>("level", 100));
        }
        catch (std::exception &e)
        {
            ERROR("Unable to init module {" << module_info.name_ << "}: " << e.what());
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

    INFO("Attempting to load module nammed " << module_name << " (shared lib file = " << filename << ")");
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
            modules_.insert(std::move(module_info));
            DEBUG("library file loaded (not init yet)");
            return true;
        }
    }
    ERROR("Could'nt load this module (file not found)");
    return false;
}

std::shared_ptr<DynamicLibrary> zModuleManager::load_library_file(const std::string &full_path)
{
    INFO("Loading library at: " << full_path);
    std::shared_ptr<DynamicLibrary> lib(new DynamicLibrary(full_path));
    try
    {
        lib->open(DynamicLibrary::RelocationMode::Now);
    }
    catch (const DynLibException &e)
    {
        ERROR("FAILURE, full path was:{" << full_path << "}: " << e.what());
        return nullptr;
    }
    return lib;
}

void zModuleManager::stopModules()
{
    for (std::set<ModuleInfo>::const_reverse_iterator itr = modules_.rbegin();
         itr != modules_.rend();
         ++itr)
    {
        INFO("Will now stop module " << itr->name_);
        assert(itr->actor_);
        itr->actor_->stop(true);
    }
}

zModuleManager::~zModuleManager()
{
    try
    {
        stopModules();
        unloadLibraries();
    }
    catch (const std::exception &e)
    {
        Leosac::print_exception(e);
    }
    catch (...)
    {
        std::cerr << "Unkown exception in zModuleManager destructor";
    }
}

zModuleManager::zModuleManager(zmqpp::context &ctx) :
        ctx_(ctx)
{

}

bool zModuleManager::start_module_helper(zmqpp::socket *socket,
        boost::property_tree::ptree ptree,
        zmqpp::context &context,
        std::function<bool(zmqpp::socket *, boost::property_tree::ptree, zmqpp::context &)> module_function)
{
    tl_log_socket = new zmqpp::socket(context, zmqpp::socket_type::push);
    tl_log_socket->connect("inproc://log-sink");

    bool ret;
    ret = module_function(socket, ptree, context);
    delete tl_log_socket;
    return ret;
}

zModuleManager::ModuleInfo::~ModuleInfo()
{
    delete actor_;
}

zModuleManager::ModuleInfo::ModuleInfo() :
        lib_(nullptr),
        actor_(nullptr)
{

}

zModuleManager::ModuleInfo::ModuleInfo(zModuleManager::ModuleInfo &&o)
{
    actor_ = o.actor_;
    lib_ = o.lib_;
    config_ = o.config_;
    name_ = o.name_;

    o.actor_ = nullptr;
    o.lib_ = nullptr;
}
