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

#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <functional>

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
    virtual ~Task()
    {
    }
    Task(const Task &) = delete;
    Task(Task &&)      = delete;
    Task &operator=(const Task &) = delete;
    Task &operator=(Task &&) = delete;

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

    bool succeed() const;

    std::exception_ptr get_exception() const;

    /**
     * Set a callback that will be invoked when the tasks is completed, no
     * matter if it succeeded or not.
     *
     * @note: Status dependent callback (`on_success`/`on_failure`) will
     * be called before the `on_complete` callback.
     */
    template <typename Callback>
    void set_on_completion(Callback c)
    {
        on_completion_ = [=]() { c(); };
    }

    /**
     * Set a callback that will be invoked if and when the task succeed.
     *
     * @note: Status dependent callback (`on_success`/`on_failure`) will
     * be called before the `on_complete` callback.
     */
    template <typename Callback>
    void set_on_success(Callback c)
    {
        on_success_ = [=]() { c(); };
    }

    /**
     * Set a callback that will be invoked if and when the task fails.
     *
     * @note: Status dependent callback (`on_success`/`on_failure`) will
     * be called before the `on_complete` callback.
     */
    template <typename Callback>
    void set_on_failure(Callback c)
    {
        on_failure_ = [=]() { c(); };
    }

    const std::string &get_guid() const;

  private:
    virtual bool do_run() = 0;

    std::function<void(void)> on_completion_;
    std::function<void(void)> on_success_;
    std::function<void(void)> on_failure_;

    bool success_;
    std::exception_ptr eptr_;

    std::mutex mutex_;
    std::atomic_bool complete_;
    std::condition_variable cv_;
    std::string guid_;
};
}
}
