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

#include "core/audit/AuditFactory.hpp"
#include "core/audit/AccessPointEvent.hpp"
#include "core/audit/CredentialEvent.hpp"
#include "core/audit/DoorEvent.hpp"
#include "core/audit/GroupEvent.hpp"
#include "core/audit/ScheduleEvent.hpp"
#include "core/audit/UpdateEvent.hpp"
#include "core/audit/UserEvent.hpp"
#include "core/audit/UserGroupMembershipEvent.hpp"
#include "core/audit/WSAPICall.hpp"
#include "core/audit/ZoneEvent.hpp"
#include "core/auth/AccessPoint.hpp"
#include "core/auth/Door.hpp"
#include "core/auth/Group.hpp"
#include "core/auth/IZone.hpp"
#include "core/auth/User.hpp"
#include "core/update/IUpdate.hpp"
#include "tools/AssertCast.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Audit;

IUserEventPtr Factory::UserEvent(const DBPtr &database, Auth::UserPtr target_user,
                                 IAuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");
    ASSERT_LOG(target_user, "Target user must be non null.");
    ASSERT_LOG(target_user->id(), "Target user must be already persisted.");
    ASSERT_LOG(parent, "Parent must be non null.");
    ASSERT_LOG(parent->id(), "Parent must be already persisted.");

    AuditEntryPtr parent_odb = std::dynamic_pointer_cast<AuditEntry>(parent);
    ASSERT_LOG(parent_odb, "Parent object was not an instance of AuditEntry");

    return Audit::UserEvent::create(database, target_user, parent_odb);
}

IGroupEventPtr Factory::GroupEvent(const DBPtr &database,
                                   Auth::GroupPtr target_group,
                                   IAuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");
    ASSERT_LOG(target_group, "Target group must be non null.");
    ASSERT_LOG(target_group->id(), "Target group must be already persisted.");
    ASSERT_LOG(parent, "Parent must be non null.");
    ASSERT_LOG(parent->id(), "Parent must be already persisted.");

    AuditEntryPtr parent_odb = std::dynamic_pointer_cast<AuditEntry>(parent);
    ASSERT_LOG(parent_odb, "Parent object was not an instance of AuditEntry");

    return Audit::GroupEvent::create(database, target_group, parent_odb);
}

IWSAPICallPtr Factory::WSAPICall(const DBPtr &database)
{
    ASSERT_LOG(database, "Database cannot be null.");

    return Audit::WSAPICall::create(database);
}

IUserGroupMembershipEventPtr
Factory::UserGroupMembershipEvent(const DBPtr &database, Auth::GroupPtr target_group,
                                  Auth::UserPtr target_user, IAuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");
    ASSERT_LOG(target_group, "Group shall not be null.");
    ASSERT_LOG(target_group->id(), "Group must be already persisted.");
    ASSERT_LOG(target_user, "User shall not be null.");
    ASSERT_LOG(target_user->id(), "User must be already persisted.");

    AuditEntryPtr parent_odb = std::dynamic_pointer_cast<AuditEntry>(parent);
    ASSERT_LOG(parent_odb, "Parent object was not an instance of AuditEntry");

    return Audit::UserGroupMembershipEvent::create(database, target_group,
                                                   target_user, parent_odb);
}

ICredentialEventPtr Factory::CredentialEventPtr(const DBPtr &database,
                                                Cred::ICredentialPtr target_cred,
                                                IAuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");
    ASSERT_LOG(target_cred, "Credential shall not be null.");
    ASSERT_LOG(target_cred->id(), "Credential must be already persisted.");

    AuditEntryPtr parent_odb = std::dynamic_pointer_cast<AuditEntry>(parent);
    ASSERT_LOG(parent_odb, "Parent object was not an instance of AuditEntry");

    return Audit::CredentialEvent::create(database, target_cred, parent_odb);
}

IScheduleEventPtr Factory::ScheduleEvent(const DBPtr &database,
                                         Tools::ISchedulePtr target_sched,
                                         IAuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");
    ASSERT_LOG(target_sched, "Schedule shall not be null.");
    ASSERT_LOG(target_sched->id(), "Schedule must be already persisted.");

    auto parent_odb = assert_cast<AuditEntryPtr>(parent);
    return Audit::ScheduleEvent::create(database, target_sched, parent_odb);
}

IDoorEventPtr Factory::DoorEvent(const DBPtr &database, Auth::IDoorPtr target_door,
                                 IAuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");
    ASSERT_LOG(target_door, "Target door must be non null.");
    ASSERT_LOG(target_door->id(), "Target door must be already persisted.");
    ASSERT_LOG(parent, "Parent must be non null.");
    ASSERT_LOG(parent->id(), "Parent must be already persisted.");

    AuditEntryPtr parent_odb = std::dynamic_pointer_cast<AuditEntry>(parent);
    ASSERT_LOG(parent_odb, "Parent object was not an instance of AuditEntry");

    return Audit::DoorEvent::create(database, target_door, parent_odb);
}

IAccessPointEventPtr Factory::AccessPointEvent(const DBPtr &database,
                                               Auth::IAccessPointPtr target_ap,
                                               IAuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");
    ASSERT_LOG(target_ap, "Target AccessPoint must be non null.");
    ASSERT_LOG(target_ap->id(), "Target AccessPoint must be already persisted.");
    ASSERT_LOG(parent, "Parent must be non null.");
    ASSERT_LOG(parent->id(), "Parent must be already persisted.");

    AuditEntryPtr parent_odb = std::dynamic_pointer_cast<AuditEntry>(parent);
    ASSERT_LOG(parent_odb, "Parent object was not an instance of AuditEntry");

    return Audit::AccessPointEvent::create(database, target_ap, parent_odb);
}

IUpdateEventPtr Factory::UpdateEvent(const DBPtr &database,
                                     update::IUpdatePtr target_update,
                                     IAuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");
    ASSERT_LOG(target_update, "Target AccessPoint must be non null.");
    ASSERT_LOG(target_update->id(), "Target AccessPoint must be already persisted.");
    ASSERT_LOG(parent, "Parent must be non null.");
    ASSERT_LOG(parent->id(), "Parent must be already persisted.");

    AuditEntryPtr parent_odb = std::dynamic_pointer_cast<AuditEntry>(parent);
    ASSERT_LOG(parent_odb, "Parent object was not an instance of AuditEntry");

    return Audit::UpdateEvent::create(database, target_update, parent_odb);
}

IZoneEventPtr Factory::ZoneEvent(const DBPtr &database, Auth::IZonePtr target_zone,
                                 IAuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");
    ASSERT_LOG(target_zone, "Target zone must be non null.");
    ASSERT_LOG(target_zone->id(), "Target zone must be already persisted.");
    ASSERT_LOG(parent, "Parent must be non null.");
    ASSERT_LOG(parent->id(), "Parent must be already persisted.");

    AuditEntryPtr parent_odb = std::dynamic_pointer_cast<AuditEntry>(parent);
    ASSERT_LOG(parent_odb, "Parent object was not an instance of AuditEntry");

    return Audit::ZoneEvent::create(database, target_zone, parent_odb);
}
