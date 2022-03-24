/*
    Copyright (C) 2014-2022 Leosac

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

#include "module_manager.hpp"
#include "core/kernel.hpp"
#include "exception/ExceptionsTools.hpp"
#include "tools/log.hpp"
#include "tools/unixfs.hpp"

using Leosac::Tools::UnixFs;
using namespace Leosac;

ModuleManager::ModuleManager(zmqpp::context &ctx, Leosac::Kernel &k)
    : ctx_(ctx)
    , config_manager_(k.config_manager())
    , core_utils_(k.core_utils())
{
}

ModuleManager::~ModuleManager()
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
        std::cerr << "Unkown exception in ModuleManager destructor" << std::endl;
    }
}

void ModuleManager::unloadLibraries()
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

void ModuleManager::initModules()
{
    for (const ModuleInfo &module_info : modules_)
    {
        // fixme ... that cast.
        initModule(const_cast<ModuleInfo *>(&module_info));
    }
}

void ModuleManager::initModule(ModuleInfo *modinfo)
{
    assert(modinfo);
    // if not null, may still be running
    assert(modinfo->actor_ == nullptr);
    assert(modinfo->lib_);
    using namespace Colorize;

    try
    {
        char *(*module_name_fct)(void) =
            (char *(*)(void))modinfo->lib_->getSymbol("get_module_name");
        assert(module_name_fct);
        std::string module_exported_name(module_name_fct());

        if (modinfo->name_ != module_exported_name)
        {
            std::stringstream error;
            error << "Missconfiguration: Configured module name doesn't match the "
                     "name exported by the module. "
                  << "(" << modinfo->name_ << " != " << module_exported_name << ")";
            ERROR(error.str());
            throw ConfigException("main configuration file", error.str());
        }

        void *symptr = modinfo->lib_->getSymbol("start_module");
        assert(symptr);
        // take the module init function and make a std::function out of it.
        std::function<bool(zmqpp::socket *, boost::property_tree::ptree,
                           zmqpp::context &, CoreUtilsPtr)>
            actor_fun = ((bool (*)(zmqpp::socket *, boost::property_tree::ptree,
                                   zmqpp::context &, CoreUtilsPtr))symptr);

        auto new_module = std::unique_ptr<zmqpp::actor>(
            new zmqpp::actor(std::bind(actor_fun, std::placeholders::_1,
                                       config_manager_.load_config(modinfo->name_),
                                       std::ref(ctx_), core_utils_)));
        modinfo->actor_ = std::move(new_module);

        INFO("Module "
             << green(modinfo->name_) << " initialized. (level = "
             << config_manager_.load_config(modinfo->name_).get<int>("level", 100)
             << ")");
    }
    catch (std::exception &e)
    {
        ERROR("Unable to init module " << red(modinfo->name_)
                                       << ". See below for "
                                          "exception information.");
        log_exception(e);
        std::throw_with_nested(ModuleException(
            "Unable to init module " + red(modinfo->name_) + ": " + e.what()));
    }
}

bool ModuleManager::initModule(const std::string &name)
{
    if (ModuleInfo *ptr = find_module_by_name(name))
    {
        initModule(ptr);
        return true;
    }
    else
    {
        WARN("Cannot find any module nammed " << name);
        return false;
    }
    return false;
}

void ModuleManager::addToPath(const std::string &dir)
{
    if (std::find(path_.begin(), path_.end(), dir) == path_.end())
        path_.push_back(dir);
}

bool ModuleManager::loadModule(const std::string &module_name)
{
    const auto &cfg      = config_manager_.load_config(module_name);
    std::string filename = cfg.get_child("file").data();

    INFO("Attempting to load module nammed "
         << module_name << " (shared lib file = " << filename << ")");
    for (const std::string &path_entry : path_)
    {
        // fixme not clean enough.
        if (UnixFs::fileExists(path_entry + "/" + filename))
        {
            ModuleInfo module_info(config_manager_);

            module_info.name_ = module_name;

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

std::shared_ptr<DynamicLibrary>
ModuleManager::load_library_file(const std::string &full_path)
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

void ModuleManager::stopModules(bool soft)
{
    for (auto itr = modules_.rbegin(); itr != modules_.rend(); ++itr)
    {
        stopModule(const_cast<ModuleInfo *>(&(*itr)), soft);
    }
    if (!soft)
    {
        modules_.clear();
        assert(modules_.size() == 0);
    }
}

void ModuleManager::stopModule(ModuleInfo *modinfo, bool soft)
{
    assert(modinfo);

    // make sure the module is running.
    if (modinfo->actor_)
    {
        INFO("Will now stop module " << modinfo->name_ << " (Soft Stop: " << soft
                                     << ")");
        // fixme i believe we may have a potential deadlock here.
        if (soft)
        {
            modinfo->actor_->stop(false);
        }
        else
        {
            modinfo->actor_->stop(true);
            modinfo->actor_ = nullptr;
        }
        // modules_.erase(*modinfo);
        // config_manager_.remove_config(modinfo->name_);
    }
    else
    {
        INFO("Not stopping module " << modinfo->name_
                                    << " as it doesn't seem to run.");
    }
}

bool ModuleManager::stopModule(const std::string &name)
{
    if (ModuleInfo *ptr = find_module_by_name(name))
    {
        stopModule(ptr);
        // config_manager_.remove_config(name);
        return true;
    }
    else
    {
        WARN("Cannot find any module nammed " << name);
        return false;
    }
    return false;
}

ModuleManager::ModuleInfo::~ModuleInfo()
{
}

ModuleManager::ModuleInfo::ModuleInfo(const Leosac::ConfigManager &cfg)
    : lib_(nullptr)
    , actor_(nullptr)
    , cfg_(cfg)
{
}

ModuleManager::ModuleInfo::ModuleInfo(ModuleManager::ModuleInfo &&o)
    : cfg_(o.cfg_)
{
    actor_ = std::move(o.actor_);
    lib_   = o.lib_;
    name_  = o.name_;

    o.actor_ = nullptr;
    o.lib_   = nullptr;
}

std::vector<std::string> ModuleManager::modules_names() const
{
    std::vector<std::string> ret;

    ret.reserve(modules_.size());
    for (auto const &module : modules_)
    {
        ret.push_back(module.name_);
    }
    return ret;
}

ModuleManager::ModuleInfo *
ModuleManager::find_module_by_name(const std::string &name) const
{
    auto itr = std::find_if(modules_.begin(), modules_.end(),
                            [&](const ModuleInfo &m) { return m.name_ == name; });

    if (itr != modules_.end())
        return const_cast<ModuleInfo *>(&(*itr));
    return nullptr;
}

bool ModuleManager::has_module(const std::string &name) const
{
    return find_module_by_name(name) != nullptr;
}

std::vector<std::string> const &ModuleManager::get_module_path() const
{
    return path_;
}

bool ModuleManager::ModuleInfo::operator<(const ModuleInfo &o) const
{
    int level_me = cfg_.load_config(name_).get<int>("level", 100);
    int level_o  = cfg_.load_config(o.name_).get<int>("level", 100);

    return level_me < level_o;
}
