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

void zModuleManager::initModules()
{
    for (const ModuleInfo &module_info : modules_)
    {
        //fixme ... that cast.
        initModule(const_cast<ModuleInfo *>(&module_info));
    }
}

void zModuleManager::initModule(ModuleInfo *modinfo)
{
    assert(modinfo);
    // if not null, may still be running
    assert(modinfo->actor_ == nullptr);
    assert(modinfo->lib_);

    try
    {
        void *symptr = modinfo->lib_->getSymbol("start_module");
        assert(symptr);
        // take the module init function and make a std::function out of it.
        std::function<bool(zmqpp::socket *, boost::property_tree::ptree, zmqpp::context &)> actor_fun =
                ((bool (*)(zmqpp::socket *, boost::property_tree::ptree, zmqpp::context &)) symptr);

        auto new_module = std::unique_ptr<zmqpp::actor>(new zmqpp::actor(std::bind(actor_fun, std::placeholders::_1,
                modinfo->config_,
                std::ref(ctx_))));
        modinfo->actor_ = std::move(new_module);

        INFO("Module {" << modinfo->name_ << "} initialized. (level = " <<
                modinfo->config_.get<int>("level", 100));
    }
    catch (std::exception &e)
    {
        ERROR("Unable to init module {" << modinfo->name_ << "}: " << e.what());
        std::throw_with_nested(ModuleException("Unable to init module {" + modinfo->name_ + "}"));
    }
}

void zModuleManager::initModule(const std::string &name)
{
    auto itr = std::find_if(modules_.begin(), modules_.end(), [&](const ModuleInfo &m)
    {
        return m.name_ == name;
    });

    if (itr != modules_.end())
    {
        //fixme cast... again
        initModule(const_cast<ModuleInfo *>(&(*itr)));
    }
    else
        WARN("Cannot find any module nammed " << name);
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
        // if a module failed to initialize, its actor will be null.
        if (itr->actor_)
        {
            INFO("Will now stop module " << itr->name_);
            itr->actor_->stop(true);
        }
        else
        {
            NOTICE("Not stopping module " << itr->name_ << " as it failed to load (or didnt load).");
        }
    }
    INFO("DONE");
}

void zModuleManager::stopModule(const std::string &name)
{
    for (std::set<ModuleInfo>::const_iterator itr = modules_.begin();
         itr != modules_.end();
         ++itr)
    {
        // if a module failed to initialize, its actor will be null.
        if (itr->actor_ && name == itr->name_)
        {
            INFO("Will now stop module (BY NAME)" << itr->name_);
            itr->actor_->stop(true);
            itr->actor_ = nullptr;
            break;
        }
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

zModuleManager::ModuleInfo::~ModuleInfo()
{

}

zModuleManager::ModuleInfo::ModuleInfo() :
        lib_(nullptr),
        actor_(nullptr)
{

}

zModuleManager::ModuleInfo::ModuleInfo(zModuleManager::ModuleInfo &&o)
{
    actor_ = std::move(o.actor_);
    lib_ = o.lib_;
    config_ = o.config_;
    name_ = o.name_;

    o.actor_ = nullptr;
    o.lib_ = nullptr;
}

std::vector<std::string> zModuleManager::modules_names() const
{
    std::vector<std::string> ret;

    ret.reserve(modules_.size());
    for (auto const &module : modules_)
    {
        ret.push_back(module.name_);
    }
    return ret;
}
