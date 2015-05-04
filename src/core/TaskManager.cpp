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

#include "TaskManager.h"
#include "Task.h"

using namespace Leosac;

void TaskManager::update()
{
    std::lock_guard<std::mutex> lg(mutex_);

    auto itr = tasks_.begin();
    while (itr != tasks_.end())
    {
        auto task = *itr;
        if (task->is_complete())
        {
            task->on_complete();
            itr = tasks_.erase(itr);
            continue;
        }
        ++itr;
    }
}

void TaskManager::schedule(const TaskPtr &task)
{
    std::lock_guard<std::mutex> lg(mutex_);

    tasks_.push_back(task);
    task->my_future = std::async(std::launch::async, std::bind(&Task::run, task));
}
