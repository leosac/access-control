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

#include <tools/log.hpp>
#include <core/tasks/GetLocalConfigVersion.h>
#include "ReplicationModule.hpp"
#include "core/Scheduler.hpp"

using namespace Leosac::Module::Replication;

ReplicationModule::ReplicationModule(zmqpp::context &ctx,
                                     zmqpp::socket *pipe,
                                     const boost::property_tree::ptree &cfg,
                                     Scheduler &sched) :
        BaseModule(ctx, pipe, cfg, sched),
        last_sync_(TimePoint::max())
{
    process_config();
}

void ReplicationModule::run()
{
    while (is_running_)
    {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now() - last_sync_).count();
        if (last_sync_ == TimePoint::max() ||
                elapsed > delay_)
        {
            replicate();
            last_sync_ = std::chrono::system_clock::now();
        }
        reactor_.poll(25);
    }
}

void ReplicationModule::process_config()
{
    delay_      = config_.get_child("module_config").get<int>("delay", 15);
    endpoint_   = config_.get_child("module_config").get<std::string>("endpoint");
    pubkey_     = config_.get_child("module_config").get<std::string>("pubkey");
}

void ReplicationModule::replicate()
{
    auto task = std::make_shared<Tasks::GetLocalConfigVersion>();
    scheduler_.enqueue(task, TargetThread::MAIN);

    task->wait();
    DEBUG("REPLICATING " << Leosac::gettid());
}
