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

#include "tools/ScheduleMapping.hpp"
#include "Credential_odb.h"
#include "Group_odb.h"
#include "User_odb.h"
#include "core/auth/Door.hpp"
#include "core/auth/User.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Tools;

ScheduleMappingId ScheduleMapping::id() const
{
    return id_;
}

bool ScheduleMapping::has_user(Auth::UserId uid) const
{
    for (const auto &lazy_weak_user : users_)
    {
        if (lazy_weak_user.object_id() == uid)
            return true;
    }
    return false;
}

bool ScheduleMapping::has_group(Auth::GroupId gid) const
{
    for (const auto &lazy_weak_group : groups_)
    {
        if (lazy_weak_group.object_id() == gid)
            return true;
    }
    return false;
}

bool ScheduleMapping::has_cred(Cred::CredentialId cid) const
{
    for (const auto &lazy_weak_cred : creds_)
    {
        if (lazy_weak_cred.object_id() == cid)
            return true;
    }
    return false;
}

bool ScheduleMapping::has_door(Auth::DoorId did) const
{
    for (const auto &lazy_weak_door : doors_)
    {
        if (lazy_weak_door.object_id() == did)
            return true;
    }
    return false;
}

bool ScheduleMapping::has_user_indirect(Auth::UserPtr user) const
{
    // Has user
    if (has_user(user->id()))
        return true;

    // Has group
    for (const auto &group_membership : user->group_memberships())
    {
        if (has_group(group_membership->group_id()))
            return true;
    }

    // Has credential
    for (const auto &lazy_credentials : user->lazy_credentials())
    {
        if (has_cred(lazy_credentials.object_id()))
            return true;
    }
    return false;
}

void ScheduleMapping::add_door(const Auth::DoorLPtr &door)
{
    ASSERT_LOG(door, "Cannot add a null door to a ScheduleMapping.");

    doors_.push_back(door);
    if (door.get_eager())
        door->schedule_mapping_added(shared_from_this());
}
