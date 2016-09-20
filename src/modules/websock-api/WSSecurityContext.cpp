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

#include "WSSecurityContext.hpp"
#include "core/auth/Group.hpp"
#include "core/auth/User.hpp"
#include "tools/db/DBService.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Module;
using namespace Leosac::Module::WebSockAPI;

WSSecurityContext::WSSecurityContext(DBServicePtr dbsrv, Auth::UserId id)
    : SecurityContext(dbsrv)
    , user_id_(id)
{
}

bool WSSecurityContext::check_permission(SecurityContext::Action action,
                                         const ActionParam &ap) const
{
    // Simply put: Administrator can do everything, without any permission check.
    if (is_admin())
        return true;

    switch (action)
    {
    case Action::USER_CREATE:
        return is_admin();
    case Action::USER_READ:
        return can_read_user(ap.user);
    case Action::USER_READ_EMAIL:
        return can_read_user_detail(ap.user);
    case Action::USER_UPDATE:
        return can_update_user(ap.user);

    case Action::GROUP_CREATE:
        return true;
    case Action::GROUP_READ:
        return can_read_group(ap.group);
    case Action::GROUP_LIST_MEMBERSHIP:
    case Action::GROUP_DELETE:
    case Action::GROUP_UPDATE:
        return can_administrate_group(ap.group);
    case Action::MEMBERSHIP_READ:
        return can_read_membership(ap.membership);
    default:
        ASSERT_LOG(0, "Not handled.");
    }
    return false;
}

bool WSSecurityContext::can_read_group(
    const SecurityContext::GroupActionParam &gap) const
{
    if (gap.group_id == 0) // listing group.
        return true;
    Auth::GroupPtr group = dbsrv_->find_group_by_id(gap.group_id);
    return group && group->member_has(user_id_);
}

bool WSSecurityContext::is_admin() const
{
    auto user = dbsrv_->find_user_by_id(user_id_);
    if (user)
        return user->rank() == Auth::UserRank::ADMIN;
    return false;
}

bool WSSecurityContext::can_administrate_group(
    const SecurityContext::GroupActionParam &gap) const
{
    Auth::GroupRank rank;
    Auth::GroupPtr group = dbsrv_->find_group_by_id(gap.group_id);
    if (group && group->member_has(user_id_, &rank))
    {
        return rank == Auth::GroupRank::ADMIN;
    }
    return false;
}

bool WSSecurityContext::can_read_membership(
    const SecurityContext::MembershipActionParam &map) const
{
    Auth::UserGroupMembershipPtr ugm =
        dbsrv_->find_membership_by_id(map.membership_id);
    ActionParam ap;
    ap.group.group_id = ugm->group_id();
    return ugm->user_id() == user_id_ ||
           check_permission(Action::GROUP_LIST_MEMBERSHIP, ap);
}

bool WSSecurityContext::can_read_user(
    const SecurityContext::UserActionParam &uap) const
{
    Auth::UserPtr user = dbsrv_->find_user_by_id(uap.user_id);
    if (user)
    {
        if (user->id() == user_id_) // self
            return true;
        for (const auto &membership : user->group_memberships())
        {
            ActionParam ap;
            ap.group.group_id = membership->group_id();
            if (check_permission(Action::GROUP_LIST_MEMBERSHIP, ap))
                return true;
        }
    }
    return false;
}

bool WSSecurityContext::can_read_user_detail(const UserActionParam &uap) const
{
    return uap.user_id == user_id_;
}

bool WSSecurityContext::can_update_user(
    const SecurityContext::UserActionParam &uap) const
{
    return uap.user_id == user_id_;
}
