//
// Created by xaqq on 5/5/15.
//

#include <assert.h>
#include <future>
#include "Scheduler.hpp"

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

void Scheduler::update(TargetThread me)
{
    mutex_.lock();
    auto &queue = queues_[me];
    bool run = queue.size();
    mutex_.unlock();

    while (run)
    {
        mutex_.lock();
        auto task = queue.front();
        queue.pop();
        run = queue.size();
        mutex_.unlock();
        task->run();
    }
}

void Scheduler::register_thread(TargetThread me)
{
    std::lock_guard<std::mutex> lg(mutex_);
    assert(queues_.count(me) == 0);
    queues_[me];
}
