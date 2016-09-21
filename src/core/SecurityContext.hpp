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
#include "tools/db/db_fwd.hpp"

namespace Leosac
{
/**
 * A SecurityContext is used to query permission while doing
 * an operation.
 *
 * For example, JSON serializers could use a SecurityContext to determine what
 * to serialize.
 *
 * todo: fix doc
 */
class SecurityContext
{
  public:
    enum class Action
    {
        USER_CREATE,
        USER_READ,
        USER_READ_EMAIL,
        USER_UPDATE,
        /**
         * Editing rank means being able to become administrator.
         */
        USER_UPDATE_RANK,
        USER_DELETE,

        GROUP_CREATE,
        GROUP_READ,
        GROUP_UPDATE,
        GROUP_DELETE,
        /**
         * Ability to list member of a group.
         * The ability to list membership gives USER_READ access against
         * the user whose membership is listed.
         */
        GROUP_LIST_MEMBERSHIP,
        GROUP_MEMBERSHIP_JOINED,
        GROUP_MEMBERSHIP_LEFT,
        MEMBERSHIP_READ,
    };

    struct GroupActionParam
    {
        Auth::GroupId group_id;
    };

    struct UserActionParam
    {
        Auth::UserId user_id;
    };

    struct MembershipActionParam
    {
        Auth::UserGroupMembershipId membership_id;
        Auth::GroupId group_id; // for create/delete
        Auth::UserId user_id;   // for create/delete
        Auth::GroupRank rank;   // for create
    };

    union ActionParam {
        GroupActionParam group;
        MembershipActionParam membership;
        UserActionParam user;
    };

    SecurityContext(DBServicePtr dbsrv);

    virtual bool check_permission(Action a, const ActionParam &ap) const;

  protected:
    DBServicePtr dbsrv_;
};

/**
 * A security context for system operation.
 *
 * Basically, this security context always grants all permissions.
 */
class SystemSecurityContext : public SecurityContext
{
  public:
    SystemSecurityContext(DBServicePtr dbsrv);

    static SecurityContext &instance();
    virtual bool check_permission(Action a, const ActionParam &ap) const override;
};
}
