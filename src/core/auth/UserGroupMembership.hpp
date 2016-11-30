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

#pragma once

#include "core/auth/AuthFwd.hpp"
#include "tools/db/database.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <set>

namespace Leosac
{
namespace Auth
{
/**
 * Describe the membership of an User with regroup to a Group.
 * This class is "database enabled".
 *
 * @note A membership is deleted on cascade when either its Group or
 * its User is deleted.
 */
#pragma db object optimistic
class UserGroupMembership
{
  public:
    UserGroupMembership();

    const UserGroupMembershipId &id() const;

    const UserLWPtr &user() const;
    UserId user_id() const;
    void user(UserPtr user);

    const GroupLWPtr group() const;
    GroupId group_id() const;
    void group(GroupPtr group);

    const boost::posix_time::ptime &timestamp() const;

    GroupRank rank() const;
    void rank(const GroupRank &rank);

  private:
#pragma db id auto
    UserGroupMembershipId id_;

#pragma db not_null
#pragma db on_delete(cascade)
    UserLWPtr user_;

#pragma db not_null
#pragma db on_delete(cascade)
    GroupLWPtr group_;

#pragma db not_null
    boost::posix_time::ptime timestamp_;

#pragma db not_null
    GroupRank rank_;

#pragma db version
    const size_t version_;

    friend class odb::access;
};

/**
 * Compares 2 UserGroupMembership object.
 *
 * If the `user` and `group` attribute of any of the compared memberships is not yet
 * persisted, we fallback on object's address comparison.
 */
struct UserGroupMembershipComparator
{
    bool operator()(const UserGroupMembershipPtr &m1,
                    const UserGroupMembershipPtr &m2) const;
};

using UserGroupMembershipSet =
    std::set<UserGroupMembershipPtr, UserGroupMembershipComparator>;
}
}

#ifdef ODB_COMPILER
#include "core/auth/Group.hpp"
#include "core/auth/User.hpp"
#endif
