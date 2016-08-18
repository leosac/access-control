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

#include "LeosacFwd.hpp"
#include "Task.hpp"

namespace Leosac
{
namespace Tasks
{
/**
 * Run in the main thread and retrieve the current configuration version.
 *
 * This is done by querying the kernel's configuration manager.
 */
class GetLocalConfigVersion : public Task
{
  public:
    GetLocalConfigVersion(Kernel &k);
    uint64_t config_version_;

  private:
    virtual bool do_run() override;

    Kernel &kernel_;
};
}
}
