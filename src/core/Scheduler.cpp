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

#include "Scheduler.hpp"
#include "core/tasks/Task.hpp"
#include <assert.h>
#include <future>
#include <functional>

using namespace Leosac;
using namespace Leosac::Tasks;

void Scheduler::enqueue(TaskPtr t, TargetThread policy)
{
    if (policy == TargetThread::POOL)
    {
        std::thread(std::bind(&Task::run, t)).detach();
    }
    else
    {
        std::lock_guard<std::mutex> lg(mutex_);
        queues_[policy].push(t);
    }
}

void Scheduler::update(TargetThread me) noexcept
{
    mutex_.lock();
    auto &queue = queues_[me];
    int run     = queue.size();
    mutex_.unlock();

    while (run)
    {
        mutex_.lock();
        auto task = queue.front();
        queue.pop();
        mutex_.unlock();
        task->run();
        run--;
    }
}

void Scheduler::register_thread(TargetThread me)
{
    std::lock_guard<std::mutex> lg(mutex_);
    assert(queues_.count(me) == 0);
    queues_[me];
}

Scheduler::Scheduler(Kernel *kptr)
    : kptr_(kptr)
{
}

Kernel &Scheduler::kernel()
{
    assert(kptr_);
    return *kptr_;
}
