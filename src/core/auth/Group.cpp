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

#include "Group.hpp"
#include "Group_odb.h"
#include "UserGroupMembership_odb.h"
#include "core/auth/UserGroupMembership.hpp"
#include "tools/log.hpp"

Leosac::Auth::Group::Group()
    : version_(0)
{
}

Leosac::Auth::Group::Group(const std::string &group_name)
    : name_(group_name)
    , version_(0)
{
}

const std::string &Leosac::Auth::Group::name() const
{
    return name_;
}

std::vector<Leosac::Auth::UserPtr> const &Leosac::Auth::Group::members() const
{
    loaded_members_.clear();
    for (const auto &membership : membership_)
    {
        if (membership->user().get_eager())
            loaded_members_.push_back(membership->user().get_eager());
    }
    return loaded_members_;
}

void Leosac::Auth::Group::member_add(Leosac::Auth::UserPtr m)
{
    // members_.push_back(m);
    // Create a new UserGroupMembership describing the relationship.
    auto ugm = std::make_shared<UserGroupMembership>();
    ugm->user(m);
    ugm->group(shared_from_this());
    membership_.insert(ugm);
    ERROR("THERE IS " << membership_.size() << " items in set.");
}

Leosac::Auth::IAccessProfilePtr Leosac::Auth::Group::profile()
{
    return profile_;
}

void Leosac::Auth::Group::profile(Leosac::Auth::IAccessProfilePtr p)
{
    profile_ = p;
}

Leosac::Auth::GroupId Leosac::Auth::Group::id() const
{
    return id_;
}

std::vector<Leosac::Auth::UserLPtr> Leosac::Auth::Group::lazy_members() const
{
    std::vector<UserLPtr> members;
    for (const auto &membership : membership_)
    {
        ASSERT_LOG(membership->group().object_id() == id_,
                   "Membership doesn't point to self.");
        members.push_back(membership->user());
    }
    return members;
}

void Leosac::Auth::Group::name(const std::string &name)
{
    name_ = name;
}

void Leosac::Auth::Group::odb_callback(odb::callback_event e,
                                       odb::database &db) const
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

const Leosac::Auth::UserGroupMembershipSet &
Leosac::Auth::Group::user_memberships() const
{
    return membership_;
}
