/*
    Copyright (C) 2014-2022 Leosac

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

#include "core/auth/AuthFwd.hpp"
#include "core/auth/AuthTarget.hpp"
#include "core/auth/Interfaces/IAccessProfile.hpp"
#include "tools/SingleTimeFrame.hpp"
#include <chrono>
#include <map>
#include <memory>
#include <vector>

namespace Leosac
{
namespace Auth
{

/**
* Concrete implementation of a simple access control class.
*/
class SimpleAccessProfile : public IAccessProfile
{
  public:
    virtual bool isAccessGranted(const std::chrono::system_clock::time_point &date,
                                 AuthTargetPtr target) override;

    /**
    * Adds a schedule where access to a given target is allowed.
    */
    virtual void addAccessSchedule(AuthTargetPtr target,
                                   const Tools::IScheduleCPtr &sched);

    virtual const std::vector<Tools::IScheduleCPtr> &defaultSchedules() const;

    /**
    * Returns the map of schedule for each target (except the default target)
    */
    virtual const std::map<std::string, std::vector<Tools::IScheduleCPtr>> &
    schedules() const;

    /**
     * Returns the number of schedule associated with the profile.
     */
    virtual size_t schedule_count() const override;

  protected:
    /**
    * Map target name to target's schedules.
    */
    std::map<std::string, std::vector<Tools::IScheduleCPtr>> schedules_;

    /**
    * Schedule for default target.
    */
    std::vector<Tools::IScheduleCPtr> default_schedule_;
};
}
}
