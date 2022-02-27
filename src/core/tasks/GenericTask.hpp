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

#include "LeosacFwd.hpp"
#include "Task.hpp"
#include <functional>

namespace Leosac
{
namespace Tasks
{
/**
 * This task type is used whenever a lambda is passed to the scheduler.
 * The scheduler use std::function to type-erase the callable object so that
 * it can fit into GenericTask.
 */
class GenericTask : public Task
{
  public:
    GenericTask(const std::function<bool(void)> &fct);

    template <typename Callable>
    static TaskPtr build(const Callable &callable)
    {
        return std::make_shared<Tasks::GenericTask>(callable);
    }

  private:
    virtual bool do_run();

    std::function<bool(void)> fct_;
};
}
}
