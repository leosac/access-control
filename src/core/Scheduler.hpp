//
// Created by xaqq on 5/5/15.
//

#ifndef SCHED_SCHEDULER_H
#define SCHED_SCHEDULER_H

#include "tasks/Task.hpp"
#include <queue>
#include <map>
#include <thread>
#include <mutex>
#include <LeosacFwd.hpp>

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
     * The schedule is fully thread-safe.
     */
    class Scheduler
    {
    public:
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
        void update(TargetThread me);

        /**
         * This is currently useless.
         */
        void register_thread(TargetThread me);

    private:
        using TaskQueue     = std::queue<Tasks::TaskPtr>;
        using TaskQueueMap  = std::map<TargetThread, TaskQueue>;

        /**
         * The internal queues of tasks.
         *
         * Each target thread has its own queue. Tasks scheduled to run
         * on `POOL` are not queued here, since they run on some detached thread
         * we don't care about.
         */
        TaskQueueMap queues_;

        mutable std::mutex mutex_;
    };

}

#endif //SCHED_SCHEDULER_H
