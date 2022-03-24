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

#pragma once

#include <cstddef>
#include <flagset.hpp>
#include <memory>
#include <odb/lazy-ptr.hxx>

namespace Leosac
{
namespace Audit
{
using AuditEntryId = unsigned long;

class Factory;

/**
 * Various Audit interface forward declaration.
 */

class IAuditEntry;
using IAuditEntryPtr = std::shared_ptr<IAuditEntry>;

class IAuditTracker;
using IAuditTrackerPtr = std::shared_ptr<IAuditTracker>;

class IUserEvent;
using IUserEventPtr = std::shared_ptr<IUserEvent>;

class IGroupEvent;
using IGroupEventPtr = std::shared_ptr<IGroupEvent>;

class IWSAPICall;
using IWSAPICallPtr = std::shared_ptr<IWSAPICall>;

class IUserGroupMembershipEvent;
using IUserGroupMembershipEventPtr = std::shared_ptr<IUserGroupMembershipEvent>;

class ICredentialEvent;
using ICredentialEventPtr = std::shared_ptr<ICredentialEvent>;

class IScheduleEvent;
using IScheduleEventPtr = std::shared_ptr<IScheduleEvent>;

class IDoorEvent;
using IDoorEventPtr = std::shared_ptr<IDoorEvent>;

class IAccessPointEvent;
using IAccessPointEventPtr = std::shared_ptr<IAccessPointEvent>;

class IUpdateEvent;
using IUpdateEventPtr = std::shared_ptr<IUpdateEvent>;

class IZoneEvent;
using IZoneEventPtr = std::shared_ptr<IZoneEvent>;

/**
 * Forward declaration of implementation class.
 */

class AuditEntry;
using AuditEntryLPtr = odb::lazy_shared_ptr<AuditEntry>;
using AuditEntryPtr  = std::shared_ptr<AuditEntry>;
using AuditEntryWPtr = std::weak_ptr<AuditEntry>;

class AuditTracker;
using AuditTrackerPtr  = std::shared_ptr<AuditTracker>;
using AuditTrackerWPtr = std::weak_ptr<AuditTracker>;

class WSAPICall;
using WSAPICallUPtr = std::unique_ptr<WSAPICall>;
using WSAPICallPtr  = std::shared_ptr<WSAPICall>;

class UserEvent;
using UserEventPtr = std::shared_ptr<UserEvent>;

class GroupEvent;
using GroupEventPtr = std::shared_ptr<GroupEvent>;

class UserGroupMembershipEvent;
using UserGroupMembershipEventPtr = std::shared_ptr<UserGroupMembershipEvent>;

class CredentialEvent;
using CredentialEventPtr = std::shared_ptr<CredentialEvent>;

class ScheduleEvent;
using ScheduleEventPtr = std::shared_ptr<ScheduleEvent>;

class DoorEvent;
using DoorEventPtr = std::shared_ptr<DoorEvent>;

class AccessPointEvent;
using AccessPointEventPtr = std::shared_ptr<AccessPointEvent>;

class UpdateEvent;
using UpdateEventPtr = std::shared_ptr<UpdateEvent>;

class ZoneEvent;
using ZoneEventPtr = std::shared_ptr<ZoneEvent>;

enum class EventType
{
    WSAPI_CALL,
    USER_CREATED,
    USER_DELETED,
    USER_EDITED,
    USER_PASSWORD_CHANGED,
    /**
     * An attempt to change the password failed.
     */
    USER_PASSWORD_CHANGE_FAILURE,

    GROUP_CREATED,
    GROUP_UPDATED,
    GROUP_DELETED,
    /**
     * Someone joined the group.
     */
    GROUP_MEMBERSHIP_JOINED,
    /**
     * Someone left the group.
     */
    GROUP_MEMBERSHIP_LEFT,

    CREDENTIAL_DELETED,
    CREDENTIAL_CREATED,
    CREDENTIAL_UPDATED,

    SCHEDULE_CREATED,
    SCHEDULE_DELETED,
    SCHEDULE_UPDATED,

    DOOR_CREATED,
    DOOR_UPDATED,
    DOOR_DELETED,
    /**
     * This event is linked to door. It is generated
     * by the Schedule CRUD manager. It's used to let a door
     * know that it may have been removed from a mapping.
     *
     * Fixme: Currently this event is triggered for every door in a mapping
     * everytime a schedule is updated.
     */
    MAPPING_MAY_HAVE_CHANGED,

    ACCESS_POINT_CREATED,
    ACCESS_POINT_UPDATED,
    ACCESS_POINT_DELETED,

    UPDATE_CREATED,
    UPDATE_ACKED,
    UPDATE_CANCELLED,

    ZONE_CREATED,
    ZONE_UPDATED,
    ZONE_DELETED,

    LAST__
};

using EventMask = FlagSet<EventType>;
}
}
