/*
    Copyright (C) 2014-2022 Leosac

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

#include "UserGroupMembershipEvent.hpp"
#include "core/audit/UserGroupMembershipEvent_odb.h"
#include "tools/db/OptionalTransaction.hpp"
#include "tools/log.hpp"
#include <tools/JSONUtils.hpp>

using namespace Leosac;
using namespace Leosac::Audit;

void UserGroupMembershipEvent::target_user(Auth::UserPtr user)
{
    target_user_    = user;
    target_user_id_ = user->id();
}

void UserGroupMembershipEvent::target_group(Auth::GroupPtr group)
{
    target_group_    = group;
    target_group_id_ = group->id();
}

UserGroupMembershipEventPtr
UserGroupMembershipEvent::create(const DBPtr &database, Auth::GroupPtr target_group,
                                 Auth::UserPtr target_user, AuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");
    ASSERT_LOG(target_user, "Target user must be non null.");
    ASSERT_LOG(target_user->id(), "Target user must be already persisted.");
    ASSERT_LOG(target_group, "Target group must be non null.");
    ASSERT_LOG(target_group->id(), "Target group must be already persisted.");
    ASSERT_LOG(parent, "Parent must be non null.");
    ASSERT_LOG(parent->id(), "Parent must be already persisted.");

    db::OptionalTransaction t(database->begin());
    Audit::UserGroupMembershipEventPtr audit =
        std::shared_ptr<Audit::UserGroupMembershipEvent>(
            new Audit::UserGroupMembershipEvent());
    audit->database_ = database;
    audit->target_user(target_user);
    audit->target_group(target_group);
    database->persist(audit);

    audit->set_parent(parent);
    database->update(audit);

    t.commit();
    return audit;
}

Auth::UserId UserGroupMembershipEvent::target_user_id() const
{
    if (target_user_.lock())
        return target_user_.object_id();
    return target_user_id_;
}

Auth::GroupId UserGroupMembershipEvent::target_group_id() const
{
    if (target_group_.lock())
        return target_group_.object_id();
    return target_group_id_;
}

std::string UserGroupMembershipEvent::generate_description() const
{
    using namespace FlagSetOperator;
    std::stringstream ss;

    if (event_mask_ & Audit::EventType::GROUP_MEMBERSHIP_JOINED)
        ss << "User " << generate_target_user_description()
           << " has joined the group " << generate_target_group_description();
    else if (event_mask_ & Audit::EventType::GROUP_MEMBERSHIP_LEFT)
        ss << "User " << generate_target_user_description() << " has left the group "
           << generate_target_group_description();

    return ss.str();
}

std::string UserGroupMembershipEvent::generate_target_user_description() const
{
    Leosac::json desc;

    desc["id"] = target_user_id();
    auto t     = target_user_.load();
    if (t)
        desc["username"] = t->username();

    return desc.dump();
}

std::string UserGroupMembershipEvent::generate_target_group_description() const
{
    Leosac::json desc;

    desc["id"] = target_group_id();
    auto t     = target_group_.load();
    if (t)
        desc["name"] = t->name();

    return desc.dump();
}
