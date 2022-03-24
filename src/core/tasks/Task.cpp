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

#include "Task.hpp"
#include "exception/ExceptionsTools.hpp"
#include "tools/GenGuid.h"
#include "tools/log.hpp"

using namespace Leosac::Tasks;

Task::Task()
    : on_completion_([]() {})
    , on_success_([]() {})
    , on_failure_([]() {})
    , success_(false)
    , eptr_(nullptr)
    , complete_(false)
    , guid_(Leosac::gen_uuid())
{
}

bool Task::is_complete() const
{
    return complete_.load(std::memory_order::memory_order_acquire);
}

void Task::run()
{
    try
    {
        success_ = do_run();
    }
    catch (std::exception &e)
    {
        WARN(
            "Task throwed an exception. It'll be swallowed but will still be stored "
            "in `eptr_`");
        Leosac::print_exception(e);
        eptr_ = std::current_exception();
    }
    if (success_)
        on_success_();
    else
        on_failure_();
    on_completion_();

    // Re-initialize the lambdas to an empty lambda.
    // This is important because it destroys copied parameter, including potential
    // shared_ptr that would otherwise cause cyclic references.
    on_success_    = []() {};
    on_failure_    = []() {};
    on_completion_ = []() {};

    {
        mutex_.lock();
        complete_.store(true, std::memory_order::memory_order_release);
        mutex_.unlock();
        cv_.notify_all();
    }
    INFO("Task ~" << guid_ << "~ completed "
                  << (success_ ? "successfully" : "with error."));
}

void Task::wait()
{
    std::unique_lock<std::mutex> ul(mutex_);
    cv_.wait(ul, [&]() {
        return complete_.load(std::memory_order::memory_order_acquire);
    });
}

bool Task::succeed() const
{
    return success_;
}

std::exception_ptr Task::get_exception() const
{
    return eptr_;
}

const std::string &Task::get_guid() const
{
    return guid_;
}
