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

#include <future>

namespace Leosac
{
    /**
     * This is a base class that represents a task. A task
     * is asynchronous.
     *
     * Usage example: When handling the remote control's SYNC_FROM command,
     * fetching the remote configuration can be slow, or can fail. This operation
     * is abstracted as a Task, so Leosac can keep running normally. If and when the
     * results become available, they are processed.
     *
     * @note: Task are run a random thread, and their on_complete method
     * is run in the main thread.
     */
    class Task
    {
    public:
        
        Task()                          = default;
        
        Task(const Task &)              = delete;
        Task(Task &&)                   = delete;
        Task &operator=(const Task &)   = delete;
        Task &operator=(Task &&)        = delete;

    private:
        virtual bool run() = 0;

        virtual bool is_complete() const final;

        /**
         * Runs on main thread on completion.
         */
        virtual void on_complete() = 0;

        std::future<bool> my_future;
        friend class TaskManager;
    };
}

