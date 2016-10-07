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
#include "Group_odb.h"
#include "User_odb.h"
#include "core/auth/Group.hpp"
#include "core/auth/User.hpp"
#include "tools/db/DBService.hpp"
#include "tools/log.hpp"
#include <tools/db/OptionalTransaction.hpp>

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
        return is_manager();
    case Action::USER_DELETE:
        return is_admin();
    case Action::USER_READ:
        return can_read_user(ap.user);
    case Action::USER_READ_EMAIL:
        return can_read_user_detail(ap.user);
    case Action::USER_UPDATE:
        return can_update_user(ap.user);
    case Action::USER_UPDATE_RANK:
    case Action::USER_MANAGE_VALIDITY:
        return is_manager();
    case Action::USER_CHANGE_PASSWORD:
        return is_manager() || is_self(ap.user.user_id);

    case Action::GROUP_SEARCH:
        return true;
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

    case Action::GROUP_MEMBERSHIP_JOINED:
        return can_create_membership(ap.membership);
    case Action::GROUP_MEMBERSHIP_LEFT:
        return can_delete_membership(ap.membership);


    case Action::CREDENTIAL_READ:
        return can_read_credential(ap.cred);

    case Action::CREDENTIAL_CREATE:
    case Action::CREDENTIAL_UPDATE:
    case Action::CREDENTIAL_DELETE:
        return is_manager();

    case Action::SCHEDULE_READ:
    case Action::SCHEDULE_CREATE:
    case Action::SCHEDULE_UPDATE:
    case Action::SCHEDULE_DELETE:
        return is_manager();

    case Action::LOG_READ:
        return is_manager();
    default:
        ASSERT_LOG(0, "Not handled.");
    }
    return false;
}

bool WSSecurityContext::can_read_group(
    const SecurityContext::GroupActionParam &gap) const
{
    if (is_manager() || gap.group_id == 0) // listing group.
        return true;
    Auth::GroupPtr group =
        dbsrv_->find_group_by_id(gap.group_id, DBService::THROW_IF_NOT_FOUND);
    return group->member_has(user_id_);
}

bool WSSecurityContext::can_administrate_group(
    const SecurityContext::GroupActionParam &gap) const
{
    if (is_manager())
        return true;
    Auth::GroupRank rank;
    Auth::GroupPtr group =
        dbsrv_->find_group_by_id(gap.group_id, DBService::THROW_IF_NOT_FOUND);
    if (group->member_has(user_id_, &rank))
    {
        return rank == Auth::GroupRank::ADMIN;
    }
    return false;
}

bool WSSecurityContext::can_read_membership(
    const SecurityContext::MembershipActionParam &map) const
{
    Auth::UserGroupMembershipPtr ugm = dbsrv_->find_membership_by_id(
        map.membership_id, DBService::THROW_IF_NOT_FOUND);

    ActionParam ap;
    ap.group.group_id = ugm->group_id();
    return ugm->user_id() == user_id_ ||
           check_permission(Action::GROUP_LIST_MEMBERSHIP, ap);
}

bool WSSecurityContext::can_read_user(const SecurityContext::UserActionParam &) const
{
    return true;
}

bool WSSecurityContext::can_read_user_detail(const UserActionParam &uap) const
{
    return uap.user_id == user_id_;
}

bool WSSecurityContext::can_update_user(
    const SecurityContext::UserActionParam &uap) const
{
    return is_manager() || uap.user_id == user_id_;
}

bool WSSecurityContext::can_create_membership(
    const SecurityContext::MembershipActionParam &map) const
{
    if (is_manager())
        return true;
    // If we are at least Operator in the group, we can add someone.
    Auth::GroupPtr group =
        dbsrv_->find_group_by_id(map.group_id, DBService::THROW_IF_NOT_FOUND);
    Auth::GroupRank rank;
    if (group->member_has(user_id_, &rank))
    {
        if (rank >= Auth::GroupRank::OPERATOR && map.rank <= rank)
        {
            // Cannot invite to a rank superior our own rank.
            return true;
        }
    }
    return false;
}

bool WSSecurityContext::can_delete_membership(
    const SecurityContext::MembershipActionParam &map) const
{
    if (is_manager())
        return true;
    db::OptionalTransaction t(dbsrv_->db()->begin());
    Auth::UserGroupMembershipPtr membership = dbsrv_->find_membership_by_id(
        map.membership_id, DBService::THROW_IF_NOT_FOUND);

    auto group       = membership->group().load();
    auto target_user = membership->user().load();
    t.commit();

    if (target_user->id() == user_id_)
        return true; // Can leave any group.

    Auth::GroupRank my_rank;
    if (group && group->member_has(user_id_, &my_rank))
    {
        if (my_rank >= Auth::GroupRank::OPERATOR && my_rank >= membership->rank())
        {
            // Cannot kick someone with a higher rank.
            return true;
        }
    }
    return false;
}

bool WSSecurityContext::is_admin() const
{
    auto user = dbsrv_->find_user_by_id(user_id_);
    if (user)
        return user->rank() == Auth::UserRank::ADMIN;
    return false;
}

bool WSSecurityContext::is_manager() const
{
    auto user = dbsrv_->find_user_by_id(user_id_);
    if (user)
        return user->rank() == Auth::UserRank::ADMIN;
    return false;
}

bool WSSecurityContext::is_self(Auth::UserId id) const
{
    return user_id_ == id;
}

bool WSSecurityContext::can_read_credential(
    const SecurityContext::CredentialActionParam &cap) const
{
    if (is_manager() || cap.credential_id == 0)
        return true;

    auto cred = dbsrv_->find_credential_by_id(cap.credential_id,
                                              DBService::THROW_IF_NOT_FOUND);
    return is_self(cred->owner_id());
}
