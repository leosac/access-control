//
// Created by xaqq on 5/5/15.
//

#include <assert.h>
#include "Task.hpp"

using namespace Leosac::Tasks;

Task::Task() :
        complete_(false)
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
        do_run();

        {
            mutex_.lock();
            complete_.store(true, std::memory_order::memory_order_release);
            mutex_.unlock();
            cv_.notify_all();
        }
    }
    catch (...)
    {
        assert(0);
        exit(-1);
    }
}

void Task::wait()
{
    std::unique_lock<std::mutex> ul(mutex_);
    cv_.wait(ul, [&] () { return complete_.load(std::memory_order::memory_order_acquire) ; });
}
