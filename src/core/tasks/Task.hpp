//
// Created by xaqq on 5/5/15.
//

#ifndef SCHED_TASK_H
#define SCHED_TASK_H

#include <memory>
#include <atomic>
#include <condition_variable>

namespace Leosac
{
    namespace Tasks
    {
        /**
         * A base class for a tasks.
         *
         * It makes use of the Non Virtual Interface pattern to let subclasses implement
         * the task code while it wraps it properly and take care of notifications.
         */
        class Task
        {
        public:
            Task();

            /**
             * Has the tasks completed its execution.
             * This method is thread safe.
             */
            bool is_complete() const;

            /**
             * Instead of spinlocking over `is_complete()` one can call `wait()`
             * to hum... wait for the task's completion.
             *
             * This is implemented using some condition_variable magic.
             */
            void wait();

            void run();

        private:
            virtual void do_run() = 0;

            std::mutex              mutex_;
            std::atomic_bool        complete_;
            std::condition_variable cv_;
        };

    }
}

#endif //SCHED_TASK_H
