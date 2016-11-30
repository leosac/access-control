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

#include "CoreAPI.hpp"
#include "Scheduler.hpp"
#include "core/tasks/GetLocalConfigVersion.hpp"
#include "kernel.hpp"
#include "tools/GenGuid.h"
#include "tools/Mail.hpp"
#include "tools/registry/GlobalRegistry.hpp"
#include <chrono>

using namespace Leosac;

CoreAPI::CoreAPI(Kernel &k)
    : kernel_(k)
{
}

uint64_t CoreAPI::config_version() const
{
    auto t = std::make_shared<Tasks::GetLocalConfigVersion>(kernel_);
    kernel_.core_utils()->scheduler().enqueue(t, TargetThread::MAIN);
    t->wait();
    ASSERT_LOG(t->succeed(), "Tasks::GetLocalConfigVersion failed.");
    return t->config_version_;
}

boost::property_tree::ptree CoreAPI::kernel_config() const
{
    boost::property_tree::ptree out;
    auto task = Tasks::GenericTask::build([&]() {
        out = kernel_.config_manager().kconfig();
        return true;
    });
    kernel_.core_utils()->scheduler().enqueue(task, TargetThread::MAIN);
    task->wait();
    ASSERT_LOG(task->succeed(),
               "Retrieving `kernel configuration` from CoreAPI failed.");

    return out;
}

uint64_t CoreAPI::uptime() const
{
    using namespace std::chrono;
    uint64_t out;
    auto task = Tasks::GenericTask::build([&]() {
        auto now = steady_clock::now();
        out      = duration_cast<seconds>(now - kernel_.start_time()).count();
        return true;
    });
    kernel_.core_utils()->scheduler().enqueue(task, TargetThread::MAIN);
    task->wait();
    ASSERT_LOG(task->succeed(), "Retrieving `uptime` from CoreAPI failed.");

    return out;
}

std::string CoreAPI::instance_name() const
{
    std::string out;
    auto task = Tasks::GenericTask::build([&]() {
        out = kernel_.config_manager().instance_name();
        return true;
    });
    kernel_.core_utils()->scheduler().enqueue(task, TargetThread::MAIN);
    task->wait();
    ASSERT_LOG(task->succeed(), "Retrieving `instance_name` from CoreAPI failed.");

    return out;
}

std::vector<std::string> CoreAPI::modules_names() const
{
    std::vector<std::string> out;
    auto task = Tasks::GenericTask::build([&]() {
        out = kernel_.module_manager().modules_names();
        return true;
    });
    kernel_.core_utils()->scheduler().enqueue(task, TargetThread::MAIN);
    task->wait();
    ASSERT_LOG(task->succeed(), "Retrieving `modules names` from CoreAPI failed.");

    return out;
}
