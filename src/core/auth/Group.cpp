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

#include "Group.hpp"
#include "Group_odb.h"
#include "UserGroupMembership_odb.h"
#include "core/auth/UserGroupMembership.hpp"
#include "tools/log.hpp"
#include <exception/ModelException.hpp>

using namespace Leosac;
using namespace Leosac::Auth;

Group::Group()
    : id_(0)
    , version_(0)
{
}

Group::Group(const std::string &group_name)
    : id_(0)
    , name_(group_name)
    , version_(0)
{
}

const std::string &Group::name() const
{
    return name_;
}

const std::vector<UserPtr> &Group::members() const
{
    for (const auto &membership : membership_)
    {
        auto ptr = membership->user().get_eager().lock();
        if (ptr &&
            std::find(loaded_members_.begin(), loaded_members_.end(), ptr) ==
                loaded_members_.end())
        {
            loaded_members_.push_back(ptr);
        }
    }
    return loaded_members_;
}

UserGroupMembershipPtr Group::member_add(UserPtr m,
                                         GroupRank rank /*= GroupRank::MEMBER*/)
{
    // Create a new UserGroupMembership describing the relationship.
    auto ugm = std::make_shared<UserGroupMembership>();
    ugm->rank(rank);
    ugm->user(m);
    ugm->group(shared_from_this());
    membership_.insert(ugm);
    return ugm;
}

IAccessProfilePtr Group::profile()
{
    return profile_;
}

void Group::profile(IAccessProfilePtr p)
{
    profile_ = p;
}

GroupId Group::id() const
{
    return id_;
}

void Group::id(const GroupId &new_id)
{
    id_ = new_id;
}

std::vector<UserLPtr> Group::lazy_members() const
{
    std::vector<UserLPtr> members;
    for (const auto &membership : membership_)
    {
        ASSERT_LOG(membership->group().object_id() == id_,
                   "Membership doesn't point to self.");
        ASSERT_LOG(membership->user().lock(), "Why is this null?");
        if (membership->user().lock())
            members.push_back(membership->user().lock());
    }
    return members;
}

void Group::name(const std::string &name)
{
    GroupValidator::validate_name(name);
    name_ = name;
}

void Group::odb_callback(odb::callback_event e, odb::database &db) const
{
    if (e == odb::callback_event::post_update ||
        e == odb::callback_event::post_persist)
    {
        for (auto &membership : membership_)
        {
            if (membership->id() == 0)
                db.persist(membership);
            else
                db.update(membership);
        }
    }
}

const UserGroupMembershipSet &Group::user_memberships() const
{
    return membership_;
}

const std::string &Group::description() const
{
    return description_;
}

void Group::description(const std::string &desc)
{
    description_ = desc;
}

bool Group::member_has(UserId user_id, GroupRank *rank_out) const
{
    for (const auto &membership : membership_)
    {
        if (membership->user().object_id() == user_id)
        {
            if (rank_out)
                *rank_out = membership->rank();
            return true;
        }
    }
    return false;
}

std::vector<Tools::ScheduleMappingLWPtr> Group::lazy_schedules_mapping() const
{
    return schedules_mapping_;
}

void Group::schedule_mapping_added(const Tools::ScheduleMappingPtr &sched_mapping)
{
    schedules_mapping_.push_back(sched_mapping);
}

void GroupValidator::validate(const Group &grp)
{
    validate_name(grp.name());
}

void GroupValidator::validate_name(const std::string &name)
{
    if (name.size() < 3 || name.size() > 50)
    {
        throw ModelException("data/attributes/name", "Length must be >=3 and <=50.");
    }
    for (const auto &c : name)
    {
        if (!isalnum(c) && (c != '_' && c != '-' && c != '.'))
        {
            throw ModelException(
                "data/attributes/name",
                BUILD_STR("Usage of unauthorized character: " << c));
        }
    }
}
