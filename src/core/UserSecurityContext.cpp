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

#include "UserSecurityContext.hpp"
#include "Group_odb.h"
#include "User_odb.h"
#include "core/auth/Group.hpp"
#include "core/auth/IDoor.hpp"
#include "core/auth/User.hpp"
#include "tools/ScheduleMapping.hpp"
#include "tools/db/DBService.hpp"
#include "tools/db/OptionalTransaction.hpp"
#include "tools/log.hpp"
#include <tools/db/MultiplexedTransaction.hpp>

using namespace Leosac;

UserSecurityContext::UserSecurityContext(DBServicePtr dbsrv, Auth::UserId id)
    : SecurityContext(dbsrv)
    , user_id_(id)
{
}

bool UserSecurityContext::check_permission_impl(SecurityContext::Action action,
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
        return can_read_schedule(ap.sched);
    case Action::SCHEDULE_CREATE:
    case Action::SCHEDULE_UPDATE:
    case Action::SCHEDULE_DELETE:
        return is_manager();
    case Action::SCHEDULE_SEARCH:
        return true;

    case Action::DOOR_READ:
        return can_read_door(ap.door);
    case Action::DOOR_CREATE:
    case Action::DOOR_UPDATE:
    case Action::DOOR_DELETE:
        return is_manager();
    case Action::DOOR_SEARCH:
        return true;

    case Action::ACCESS_POINT_READ:
    case Action::ACCESS_POINT_CREATE:
    case Action::ACCESS_POINT_UPDATE:
    case Action::ACCESS_POINT_DELETE:
    case Action::ACCESS_POINT_SEARCH:
        return is_manager();

    case Action::SMTP_GETCONFIG:
    case Action::SMTP_SETCONFIG:
    case Action::SMTP_SENDMAIL:
        return is_admin();

    case Action::LOG_READ:
        return is_manager();

    case Action::AUDIT_READ:
        return is_manager();
    case Action::AUDIT_READ_FULL:
        return is_admin();
    default:
        ASSERT_LOG(0, "Not handled.");
    }
    return false;
}

bool UserSecurityContext::can_read_group(
    const SecurityContext::GroupActionParam &gap) const
{
    if (is_manager() || gap.group_id == 0) // listing group.
        return true;
    Auth::GroupPtr group =
        dbsrv_->find_group_by_id(gap.group_id, DBService::THROW_IF_NOT_FOUND);
    return group->member_has(user_id_);
}

bool UserSecurityContext::can_administrate_group(
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

bool UserSecurityContext::can_read_membership(
    const SecurityContext::MembershipActionParam &map) const
{
    Auth::UserGroupMembershipPtr ugm = dbsrv_->find_membership_by_id(
        map.membership_id, DBService::THROW_IF_NOT_FOUND);

    ActionParam ap;
    ap.group.group_id = ugm->group_id();
    return ugm->user_id() == user_id_ ||
           check_permission(Action::GROUP_LIST_MEMBERSHIP, ap);
}

bool UserSecurityContext::can_read_user(
    const SecurityContext::UserActionParam &) const
{
    return true;
}

bool UserSecurityContext::can_read_user_detail(const UserActionParam &uap) const
{
    return is_self(uap.user_id) || is_manager();
}

bool UserSecurityContext::can_update_user(
    const SecurityContext::UserActionParam &uap) const
{
    return is_self(uap.user_id) || is_manager();
}

bool UserSecurityContext::can_create_membership(
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

bool UserSecurityContext::can_delete_membership(
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

bool UserSecurityContext::can_read_schedule(
    const SecurityContext::ScheduleActionParam &cap) const
{
    if (is_manager() || cap.schedule_id == 0)
        return true;

    Tools::ISchedulePtr sched =
        dbsrv_->find_schedule_by_id(cap.schedule_id, DBService::THROW_IF_NOT_FOUND);
    // We need to find out is assigned to the schedule.
    // If it is, we can read the schedule's data.
    // Todo: Maybe have more fine grained permission here, like being able to read
    // timeframes and door mapped but not everything.

    for (const auto &mapping : sched->mapping())
    {
        if (mapping->has_user_indirect(self()))
            return true;
    }
    return false;
}

bool UserSecurityContext::can_read_door(
    const SecurityContext::DoorActionParam &dap) const
{
    if (is_manager() || dap.door_id == 0)
        return true;

    db::MultiplexedTransaction t(dbsrv_->db()->begin());
    Auth::IDoorPtr door =
        dbsrv_->find_door_by_id(dap.door_id, DBService::THROW_IF_NOT_FOUND);
    for (const auto &mapping : door->lazy_mapping())
    {
        auto loaded_mapping = mapping.load();
        if (loaded_mapping->has_user_indirect(self()))
            return true;
    }
    return false;
}

bool UserSecurityContext::can_read_credential(
    const SecurityContext::CredentialActionParam &cap) const
{
    if (is_manager() || cap.credential_id == 0)
        return true;

    auto cred = dbsrv_->find_credential_by_id(cap.credential_id,
                                              DBService::THROW_IF_NOT_FOUND);
    return is_self(cred->owner_id());
}


bool UserSecurityContext::is_admin() const
{
    auto user = dbsrv_->find_user_by_id(user_id_);
    if (user)
        return user->rank() == Auth::UserRank::ADMIN;
    return false;
}

bool UserSecurityContext::is_manager() const
{
    auto user = dbsrv_->find_user_by_id(user_id_);
    if (user)
        return user->rank() >= Auth::UserRank::MANAGER;
    return false;
}

bool UserSecurityContext::is_self(Auth::UserId id) const
{
    return user_id_ == id;
}

Auth::UserId UserSecurityContext::user_id() const
{
    return user_id_;
}

Auth::UserPtr UserSecurityContext::self() const
{
    return dbsrv_->find_user_by_id(user_id_, DBService::THROW_IF_NOT_FOUND);
}
