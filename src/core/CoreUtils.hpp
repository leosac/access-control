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

#include "CoreAPI.hpp"
#include "LeosacFwd.hpp"
#include "tools/db/db_fwd.hpp"

namespace Leosac
{
/**
 * This class is part of Leosac::Kernel, but it only exposes thread-safe
 * functionalities that may be used by every modules.
 *
 * It currently exposes the scheduler and the configuration checker object along
 * with some command line parameter value.
 *
 * A pointer to this object is passed to modules when they are created.
 */
class CoreUtils
{
  public:
    CoreUtils();
    CoreUtils(Kernel *kptr, SchedulerPtr sched, ConfigCheckerPtr cfgcheck,
              bool strict_mode);

    CoreUtils(const CoreUtils &) = delete;
    CoreUtils(CoreUtils &&)      = delete;
    CoreUtils &operator=(const CoreUtils &) = delete;
    CoreUtils &operator=(CoreUtils &&) = delete;
    virtual ~CoreUtils()
    {
    }

    Scheduler &scheduler();
    ConfigChecker &config_checker();
    Kernel &kernel();
    DBPtr database();

    /**
     * Instantiate a new, ready to use, CoreAPI object.
     */
    CoreAPI core_api();

    /**
     * Are we running in strict mode ?
     */
    bool is_strict() const;

  private:
    Kernel *kptr_;
    SchedulerPtr scheduler_;
    ConfigCheckerPtr config_checker_;
    bool strict_mode_;

    /**
     * Gives the `Kernel` class full control.
     */
    friend class Kernel;
};
}
