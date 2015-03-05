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

#include <cassert>
#include "ProfileMerger.hpp"
#include "SimpleAccessProfile.hpp"

using namespace Leosac::Auth;

using SingleTimeFrame = Leosac::Tools::SingleTimeFrame;

IAccessProfilePtr ProfileMerger::merge(std::shared_ptr<const IAccessProfile> p1, std::shared_ptr<const IAccessProfile> p2)
{
    assert(p1 && p2);
    const SimpleAccessProfile *t_p1 = dynamic_cast<const SimpleAccessProfile *>(p1.get());
    const SimpleAccessProfile *t_p2 = dynamic_cast<const SimpleAccessProfile *>(p2.get());
    assert(t_p1 && t_p2);

    SimpleAccessProfilePtr result(new SimpleAccessProfile());

    // merge strategy: simply add all schedule from both profile.

    for (const auto &schedule : t_p1->defaultSchedules())
    {
        result->addAccessSchedule(nullptr, schedule);
    }
    for (const auto &schedule : t_p2->defaultSchedules())
    {
        result->addAccessSchedule(nullptr, schedule);
    }

    std::string door_name;
    std::vector<Tools::Schedule> schedules;
    for (const auto &name_sched_pair : t_p1->schedules())
    {
        std::tie(door_name, schedules) = name_sched_pair;
        // hack
        AuthTargetPtr target(new AuthTarget(door_name));
        for (const Tools::Schedule &sched : schedules)
            result->addAccessSchedule(target, sched);
    }
    for (const auto &name_sched_pair : t_p2->schedules())
    {
        std::tie(door_name, schedules) = name_sched_pair;
        // hack
        AuthTargetPtr target(new AuthTarget(door_name));
        for (const Tools::Schedule &sched : schedules)
            result->addAccessSchedule(target, sched);
    }

    return result;
}
