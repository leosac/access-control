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

#include "core/auth/UserGroupMembership.hpp"
#include "Group_odb.h"
#include "User_odb.h"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Auth;

UserGroupMembership::UserGroupMembership()
    : version_(0)
{
    timestamp_ = boost::posix_time::second_clock::local_time();
    rank_      = GroupRank::MEMBER;
}

const UserGroupMembershipId &UserGroupMembership::id() const
{
    return id_;
}

const UserLWPtr &UserGroupMembership::user() const
{
    return user_;
}

void UserGroupMembership::user(UserPtr user)
{
    user_ = user;
}

const GroupLWPtr UserGroupMembership::group() const
{
    return group_;
}

void UserGroupMembership::group(GroupPtr group)
{
    group_ = group;
}

const boost::posix_time::ptime &UserGroupMembership::timestamp() const
{
    return timestamp_;
}

GroupRank UserGroupMembership::rank() const
{
    return rank_;
}

bool UserGroupMembershipComparator::
operator()(const UserGroupMembershipPtr &m1, const UserGroupMembershipPtr &m2) const
{
    return std::make_pair(m1->user().object_id(), m1->group().object_id()) <
           std::make_pair(m2->user().object_id(), m2->group().object_id());
}
