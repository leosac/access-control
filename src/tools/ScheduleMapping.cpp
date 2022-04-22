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

#include "tools/ScheduleMapping.hpp"
#include "AssertCast.hpp"
#include "core/auth/Door.hpp"
#include "core/auth/Zone.hpp"
#include "core/auth/Group.hpp"
#include "core/auth/Group_odb.h"
#include "core/auth/User.hpp"
#include "core/auth/User_odb.h"
#include "core/credentials/Credential.hpp"
#include "core/credentials/Credential_odb.h"
#include "tools/ScheduleMapping_odb.h"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Tools;

ScheduleMappingId ScheduleMapping::id() const
{
    return id_;
}

const std::string &ScheduleMapping::alias() const
{
    return alias_;
}

void ScheduleMapping::alias(const std::string &a)
{
    alias_ = a;
}

ScheduleId ScheduleMapping::schedule_id() const
{
    if (schedule_.lock())
        return schedule_.object_id();
    return 0;
}

ScheduleLWPtr ScheduleMapping::schedule() const
{
    return schedule_;
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

bool ScheduleMapping::has_zone(Auth::ZoneId zid) const
{
    for (const auto &lazy_weak_zone : zones_)
    {
        if (lazy_weak_zone.object_id() == zid)
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

void ScheduleMapping::add_zone(const Auth::ZoneLPtr &zone)
{
    ASSERT_LOG(zone, "Cannot add a null zone to a ScheduleMapping.");

    zones_.push_back(zone);
    if (zone.get_eager())
        zone->schedule_mapping_added(shared_from_this());
}

void ScheduleMapping::add_user(const Auth::UserLPtr &user)
{
    ASSERT_LOG(user, "Cannot add a null user to a ScheduleMapping.");

    users_.push_back(user);
    if (user.get_eager())
        user->schedule_mapping_added(shared_from_this());
}

void ScheduleMapping::add_group(const Auth::GroupLPtr &group)
{
    ASSERT_LOG(group, "Cannot add a null group to a ScheduleMapping.");

    groups_.push_back(group);
    if (group.get_eager())
        group->schedule_mapping_added(shared_from_this());
}

void ScheduleMapping::add_credential(const Cred::CredentialLPtr &cred)
{
    ASSERT_LOG(cred, "Cannot add a null credential to a ScheduleMapping.");

    creds_.push_back(cred);
    if (cred.get_eager())
        cred->schedule_mapping_added(shared_from_this());
}

const std::vector<Auth::UserLWPtr> &ScheduleMapping::users() const
{
    return users_;
}

const std::vector<Auth::GroupLWPtr> &ScheduleMapping::groups() const
{
    return groups_;
}

const std::vector<Cred::CredentialLWPtr> &ScheduleMapping::credentials() const
{
    return creds_;
}

const std::vector<Auth::DoorLWPtr> &ScheduleMapping::doors() const
{
    return doors_;
}

const std::vector<Auth::ZoneLWPtr> &ScheduleMapping::zones() const
{
    return zones_;
}

void ScheduleMapping::clear_doors()
{
    doors_.clear();
}

void ScheduleMapping::clear_zones()
{
    zones_.clear();
}

void ScheduleMapping::clear_users()
{
    users_.clear();
}

void ScheduleMapping::clear_groups()
{
    groups_.clear();
}

void ScheduleMapping::clear_credential()
{
    creds_.clear();
}

size_t ScheduleMapping::odb_version() const
{
    return odb_version_;
}
