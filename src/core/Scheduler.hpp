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

#pragma once

#include <queue>
#include <map>
#include <thread>
#include <mutex>
#include <core/tasks/GenericTask.hpp>
#include "LeosacFwd.hpp"

namespace Leosac
{

enum class TargetThread
{
    MAIN,
    POOL,
};

/**
 * This is a scheduler that is used internally to schedule asynchronous / long
 * running tasks.
 *
 * It currently support running a task on the main thread, or in some
 * random thread.
 *
 * The scheduler is fully thread-safe.
 */
class Scheduler
{
  public:
    /**
     * Construct a scheduler object (generally 1 per application).
     * The `kptr` pointer should never be null, except when writing test cases.
     *
     * @note We use a pointer here to ease testing
     */
    Scheduler(Kernel *kptr);

    Scheduler(const Scheduler &) = delete;
    Scheduler(Scheduler &&)      = delete;
    Scheduler &operator=(const Scheduler &) = delete;
    Scheduler &operator=(Scheduler &&) = delete;

    template <typename Callable>
    typename std::enable_if<
        !std::is_convertible<Callable, std::shared_ptr<Tasks::Task>>::value,
        void>::type
    enqueue(const Callable &call, TargetThread policy)
    {
        enqueue(Tasks::GenericTask::build(call), policy);
    }

    /**
     * Enqueue a task, a schedule to run on thread `policy`.
     */
    void enqueue(Tasks::TaskPtr t, TargetThread policy);

    /**
     * This will run queued tasks that are scheduled to run on thread
     * `me`.
     *
     * @warning It is **important** to call this function with the correct
     * parameter.
     */
    void update(TargetThread me) noexcept;

    /**
     * This is currently useless.
     */
    void register_thread(TargetThread me);

    /**
     * Retrieve the kernel reference associated with the scheduler.
     * This function will crash the application if the kernel pointer is null.
     */
    Kernel &kernel();

  private:
    using TaskQueue    = std::queue<Tasks::TaskPtr>;
    using TaskQueueMap = std::map<TargetThread, TaskQueue>;

    /**
     * The internal queues of tasks.
     *
     * Each target thread has its own queue. Tasks scheduled to run
     * on `POOL` are not queued here, since they run on some detached thread
     * we don't care about.
     */
    TaskQueueMap queues_;

    Kernel *kptr_;
    mutable std::mutex mutex_;
};
}
