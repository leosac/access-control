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
 * Validate a Group object's attributes, throwing ModelException
 * if some attributes are invalid.
 *
 * Validations rules:
 *     + `name` must contains ascii alphanumeric character, "-", "_" or ".".
 */
class GroupValidator
{
  public:
    /**
     * Validate the group's attributes.
     */
    static void validate(const GroupPtr &grp);
    static void validate_name(const std::string &name);
};

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

    const std::string &description() const;
    void description(const std::string &desc);

    const std::vector<UserPtr> &members() const;

    /**
     * Retrieve lazy pointers to members.
     */
    std::vector<UserLPtr> lazy_members() const;

    void member_add(UserPtr m, GroupRank rank = GroupRank::MEMBER);

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


    /**
     * Check if `user_id` is a member of this group.
     *
     * If it is a member, then `rank_out` (if non null) is set to the rank
     * of the user inside the group.
     */
    bool member_has(Auth::UserId user_id, GroupRank *rank_out = nullptr) const;

  private:
    friend class odb::access;
    friend class GroupValidator;

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

#pragma db not_null
#pragma db unique
#pragma db type("VARCHAR(128)")
    std::string name_;

/**
 * A (potentially long) description of the
 * group.
 */
#pragma db not_null
    std::string description_;

#pragma db transient
    IAccessProfilePtr profile_;

#pragma db version
    const size_t version_;
};
}
}

#ifdef ODB_COMPILER
#include "core/auth/User.hpp"
#endif
