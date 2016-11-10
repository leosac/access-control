/*
    Copyright (C) 2014-2016 Islog

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

#include "core/auth/Group.hpp"
#include "GroupSerializer.hpp"
#include "Schedule_odb.h"
#include "tools/JSONUtils.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Auth;

json GroupJSONSerializer::serialize(const Auth::Group &group,
                                    const SecurityContext &sc)
{
    json memberships = {};

    for (const auto &membership : group.user_memberships())
    {
        SecurityContext::ActionParam ap;
        ap.membership.membership_id = membership->id();
        if (sc.check_permission(SecurityContext::Action::MEMBERSHIP_READ, ap))
        {
            json group_info = {{"id", membership->id()},
                               {"type", "user-group-membership"}};
            memberships.push_back(group_info);
        }
    }
    std::set<Tools::ScheduleId> schedule_ids;
    json schedules = {};
    for (const Tools::ScheduleMappingLWPtr &mapping : group.lazy_schedules_mapping())
    {
        auto loaded = mapping.load();
        ASSERT_LOG(loaded, "Cannot load. Need to investigate.");
        schedule_ids.insert(loaded->schedule_id());
    }
    for (const auto &id : schedule_ids)
    {
        json sched_info = {{"id", id}, {"type", "schedule"}};
        schedules.push_back(sched_info);
    }

    json serialized = {
        {"id", group.id()},
        {"type", "group"},
        {"attributes",
         {
             {"name", group.name()}, {"description", group.description()},
         }},
        {"relationships",
         {{"memberships", {{"data", memberships}}},
          {"schedules", {{"data", schedules}}}}}};

    return serialized;
}

void GroupJSONSerializer::unserialize(Auth::Group &out, const json &in,
                                      const SecurityContext &)
{
    using namespace Leosac::JSONUtil;

    out.name(extract_with_default(in, "name", out.name()));
    out.description(extract_with_default(in, "description", out.description()));
}

std::string GroupJSONStringSerializer::serialize(const Auth::Group &in,
                                                 const SecurityContext &sc)
{
    return GroupJSONSerializer::serialize(in, sc).dump(4);
}

void GroupJSONStringSerializer::unserialize(Auth::Group &out, const std::string &in,
                                            const SecurityContext &sc)
{
    auto tmp = json::parse(in);
    GroupJSONSerializer::unserialize(out, tmp, sc);
}
