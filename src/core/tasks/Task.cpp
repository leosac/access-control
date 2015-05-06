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

#include <assert.h>
#include <exception/ExceptionsTools.hpp>
#include <tools/log.hpp>
#include "Task.hpp"

using namespace Leosac::Tasks;

Task::Task() :
        complete_(false),
        success_(false),
        eptr_(nullptr)
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
        WARN("Task throwed an exception. It'll be swallowed but will still be stored "
                     "in `eptr_`");
        Leosac::print_exception(e);
        eptr_ = std::current_exception();
    }

    {
        mutex_.lock();
        complete_.store(true, std::memory_order::memory_order_release);
        mutex_.unlock();
        cv_.notify_all();
    }
}

void Task::wait()
{
    std::unique_lock<std::mutex> ul(mutex_);
    cv_.wait(ul, [&] () { return complete_.load(std::memory_order::memory_order_acquire) ; });
}

bool Task::succeed() const
{
    return success_;
}

std::exception_ptr Task::get_exception() const
{
    return eptr_;
}
