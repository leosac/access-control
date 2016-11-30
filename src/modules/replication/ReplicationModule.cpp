/*
    Copyright (C) 2014-2016 Leosac

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

#include "ReplicationModule.hpp"
#include "core/CoreUtils.hpp"
#include "core/Scheduler.hpp"
#include "core/tasks/FetchRemoteConfig.hpp"
#include "core/tasks/GetLocalConfigVersion.hpp"
#include "core/tasks/GetRemoteConfigVersion.hpp"
#include "core/tasks/SyncConfig.hpp"
#include "tools/log.hpp"

using namespace Leosac::Module::Replication;

ReplicationModule::ReplicationModule(zmqpp::context &ctx, zmqpp::socket *pipe,
                                     const boost::property_tree::ptree &cfg,
                                     CoreUtilsPtr utils)
    : BaseModule(ctx, pipe, cfg, utils)
    , last_sync_(TimePoint::max())
{
    process_config();
}

void ReplicationModule::run()
{
    while (is_running_)
    {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                           std::chrono::system_clock::now() - last_sync_)
                           .count();
        if (last_sync_ == TimePoint::max() || elapsed > delay_)
        {
            replicate();
            last_sync_ = std::chrono::system_clock::now();
        }
        reactor_.poll(25);
    }
}

void ReplicationModule::process_config()
{
    delay_    = config_.get_child("module_config").get<int>("delay", 120);
    endpoint_ = config_.get_child("module_config").get<std::string>("endpoint");
    pubkey_   = config_.get_child("module_config").get<std::string>("pubkey");
}

void ReplicationModule::replicate()
{
    uint64_t local;
    uint64_t remote;
    bool ok = true;

    ok &= fetch_local_version(local);
    ok &= fetch_remote_version(remote);

    if (ok)
        INFO("Current cfg version = " << local << ". Remote = " << remote);
    else
    {
        ERROR("Failed to retrieve config version");
        return;
    }

    if (remote > local)
    {
        start_sync();
    }
    else
    {
        INFO("Local configuration version is either equal or greated than the "
             "remote's."
             << " Doing nothing.");
    }
}

bool ReplicationModule::fetch_local_version(uint64_t &local)
{
    auto task = std::make_shared<Tasks::GetLocalConfigVersion>(utils_->kernel());
    utils_->scheduler().enqueue(task, TargetThread::MAIN);
    task->wait();
    assert(task->succeed());

    local = task->config_version_;
    return true;
}

bool ReplicationModule::fetch_remote_version(uint64_t &remote)
{
    auto task = std::make_shared<Tasks::GetRemoteConfigVersion>(endpoint_, pubkey_);
    utils_->scheduler().enqueue(task, TargetThread::POOL);
    task->wait();

    if (!task->succeed())
    {
        if (task->get_exception())
        {
            try
            {
                std::rethrow_exception(task->get_exception());
            }
            catch (const std::exception &e)
            {
                ERROR("Fetching remote version failed: " << e.what());
                return false;
            }
        }
        return false;
    }
    remote = task->config_version_;
    return true;
}

void ReplicationModule::start_sync()
{
    INFO("Starting the synchronization process...");
    // two tasks queued. Fetch and Sync.

    auto fetch_task = std::make_shared<Tasks::FetchRemoteConfig>(endpoint_, pubkey_);

    auto sync_task = std::make_shared<Tasks::SyncConfig>(utils_->kernel(),
                                                         fetch_task, true, true);
    sync_task->set_on_success([]() { INFO("Synchronization complete."); });

    auto *sched = &utils_->scheduler();
    fetch_task->set_on_success(
        [=]() { sched->enqueue(sync_task, TargetThread::MAIN); });
    utils_->scheduler().enqueue(fetch_task, TargetThread::POOL);
}
