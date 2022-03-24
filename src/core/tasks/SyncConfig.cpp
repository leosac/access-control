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

#include "SyncConfig.hpp"
#include "FetchRemoteConfig.hpp"
#include "core/config/ConfigManager.hpp"
#include "core/kernel.hpp"
#include "tools/log.hpp"
#include <cassert>
#include <fstream>

using namespace Leosac;
using namespace Leosac::Tasks;

SyncConfig::SyncConfig(Kernel &kref, FetchRemoteConfigPtr fetch_task,
                       bool sync_general_config, bool autocommit)
    : kernel_(kref)
    , fetch_task_(fetch_task)
    , sync_general_config_(sync_general_config)
    , autocommit_(autocommit)
{
    INFO("Creating SyncConfig task. Guid = " << get_guid());
}

bool SyncConfig::do_run()
{
    assert(fetch_task_->is_complete());
    if (!fetch_task_->succeed())
    {
        WARN("Parent task (fetching config data) failed. Aborting SyncConfig task.");
        return false;
    }

    try
    {
        kernel_.core_utils()->config_checker().clear();
        sync_config();
    }
    catch (const std::exception &e)
    {
        ERROR("SyncConfig task had a problem " << e.what());
        return false;
    }

    return true;
}

void SyncConfig::sync_config()
{
    const RemoteConfigCollector &collector = fetch_task_->collector();
    ConfigManager backup                   = kernel_.config_manager();

    if (sync_general_config_)
    {
        INFO("Also syncing general configuration.");
        // syncing the global configure requires restart.
        kernel_.config_manager().set_kconfig(collector.general_config());
        kernel_.restart_later();
    }

    kernel_.module_manager().stopModules();
    for (const auto &name : collector.modules_list())
    {
        DEBUG("Handling module {" << name << "}");
        if (kernel_.config_manager().is_module_importable(name))
        {
            INFO("Updating config for {" << name << "}");
            kernel_.config_manager().store_config(name,
                                                  collector.module_config(name));
            // write additional file.
            for (const std::pair<std::string, std::string> &file_info :
                 collector.additional_files(name))
            {
                INFO("Writing additional config file " << file_info.first);
                std::ofstream of(file_info.first);
                of << file_info.second;
            }
        }
        else
        { // If the module is immutable (aka conf not synchronized)
            // we simply load its config from backup.
            DEBUG("Apparently {" << name
                                 << "} is immutable (declared in <no_import>)");
            if (backup.has_config(name))
            {
                // if we prevent the import of a non-loaded module, we can't
                // load from backup
                kernel_.config_manager().store_config(name,
                                                      backup.load_config(name));
            }
        }
    }

    // reload config for our local immutable modules.
    for (const auto &name : backup.get_non_importable_modules())
    {
        // Non importable can also hold "blacklisted" module that we don't want.
        // We have to check.
        if (backup.has_config(name))
        {
            DEBUG("Reloading config from backup for " << name);
            kernel_.config_manager().store_config(name, backup.load_config(name));
        }
        else
        {
            DEBUG("Not reload config from backup for " << name);
        }
    }

    assert(kernel_.module_manager()
               .modules_names()
               .empty()); // we should have 0 module loaded.
    for (const auto &name : collector.modules_list())
    {
        if (kernel_.config_manager().has_config(name))
        { // load new module.
            bool ret = kernel_.module_manager().loadModule(name);
            if (!ret)
                ERROR("Cannot load module " << name << "after synchronisation.");
            assert(ret);
        }
    }
    kernel_.config_manager().config_version(collector.remote_version());
    kernel_.module_manager().initModules();
    if (autocommit_)
    {
        INFO("Saving configuration to disk after synchronization.");
        kernel_.save_config();
    }
}
