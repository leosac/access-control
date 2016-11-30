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

#include "SimpleAccessProfile.hpp"
#include <assert.h>
#include <tools/log.hpp>

using namespace Leosac::Auth;
using SingleTimeFrame = Leosac::Tools::SingleTimeFrame;

bool SimpleAccessProfile::isAccessGranted(
    const std::chrono::system_clock::time_point &date, AuthTargetPtr target)
{
    // check "general" permissions that apply to all target
    for (const auto &sched : default_schedule_)
    {
        if (sched->is_in_schedule(date))
        {
            INFO("Access is granted through schedule '" << sched->name() << "'");
            return true;
        }
    }

    // check door specific permissions.
    if (target && schedules_[target->name()].size())
    {
        for (const auto &sched : schedules_[target->name()])
        {
            if (sched->is_in_schedule(date))
            {
                INFO("Access is granted through schedule '" << sched->name() << "'");
                return true;
            }
        }
    }
    return false;
}

void SimpleAccessProfile::addAccessSchedule(
    AuthTargetPtr target, Leosac::Tools::IScheduleCPtr const &sched)
{
    if (target)
        schedules_[target->name()].push_back(sched);
    else
        default_schedule_.push_back(sched);
}

std::map<std::string, std::vector<Leosac::Tools::IScheduleCPtr>> const &
SimpleAccessProfile::schedules() const
{
    return schedules_;
}

std::vector<Leosac::Tools::IScheduleCPtr> const &
SimpleAccessProfile::defaultSchedules() const
{
    return default_schedule_;
}
