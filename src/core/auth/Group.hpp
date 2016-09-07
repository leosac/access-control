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

#pragma once

#include "core/auth/AuthFwd.hpp"
#include "core/auth/UserGroupMembership.hpp"
#include "tools/db/database.hpp"
#include <odb/callback.hxx>
#include <string>
#include <vector>

namespace Leosac
{
namespace Auth
{

/**
* A authentication group regroup users that share permissions.
*/
#pragma db object callback(odb_callback) optimistic
class Group : public std::enable_shared_from_this<Group>
{
  public:
    Group();
    explicit Group(const std::string &group_name);

    /**
     * Retrieve the unique identifier of the group.
     */
    GroupId id() const;


    const std::string &name() const;
    void name(const std::string &name);

    const std::vector<UserPtr> &members() const;

    /**
     * Retrieve lazy pointers to members.
     */
    std::vector<UserLPtr> lazy_members() const;

    void member_add(UserPtr m);

    IAccessProfilePtr profile();

    void profile(IAccessProfilePtr p);

    /**
     * Retrieve the UserGroupMembership that this group is
     * involved with.
     *
     * While the set is always eagerly loaded, the `group()` and `user()`
     * method in each Membership will return lazy weak pointer.
     */
    const UserGroupMembershipSet &user_memberships() const;

  private:
    friend class odb::access;

    void odb_callback(odb::callback_event e, odb::database &) const;

/**
 * The group identifier.
 *
 * This is a the primary key for the object.
 */
#pragma db id auto
    GroupId id_;

#pragma db value_not_null inverse(group_)
    UserGroupMembershipSet membership_;

/**
 * This returns a vector of loaded User object.
 * When `members()` is called, this vector is populated by copying
 * some pointers from membership_.
 */
#pragma db transient
    mutable std::vector<UserPtr> loaded_members_;

    std::string name_;

#pragma db transient
    IAccessProfilePtr profile_;

#pragma db version
    const ssize_t version_;
};
}
}

#ifdef ODB_COMPILER
#include "core/auth/User.hpp"
#endif
