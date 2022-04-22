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

class IAuthEvent;
using IAuthEventPtr = std::shared_ptr<IAuthEvent>;

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

class AuthEvent;
using AuthEventPtr = std::shared_ptr<AuthEvent>;

class AccessPointEvent;
using AccessPointEventPtr = std::shared_ptr<AccessPointEvent>;

class UpdateEvent;
using UpdateEventPtr = std::shared_ptr<UpdateEvent>;

class ZoneEvent;
using ZoneEventPtr = std::shared_ptr<ZoneEvent>;

enum class EventType
{
  /**
   * Admin & System Event Types
   */
  AL_SYSTEM                         = 0x1000,
  /**
   * User Event Types
   */
  AL_USER                           = 0x2000,

  WSAPI_CALL                        = 0x00 & AL_SYSTEM,

  USER_CREATED                      = 0x10 & AL_SYSTEM,
  USER_DELETED                      = 0x11 & AL_SYSTEM,
  USER_EDITED                       = 0x12 & AL_SYSTEM,
  USER_PASSWORD_CHANGED             = 0x13 & AL_SYSTEM,
  /**
   * An attempt to change the password failed.
   */
  USER_PASSWORD_CHANGE_FAILURE      = 0x14 & AL_SYSTEM,

  GROUP_CREATED                     = 0x20 & AL_SYSTEM,
  GROUP_UPDATED                     = 0x21 & AL_SYSTEM,
  GROUP_DELETED                     = 0x22 & AL_SYSTEM,
  /**
   * Someone joined the group.
   */
  GROUP_MEMBERSHIP_JOINED           = 0x23 & AL_SYSTEM,
  /**
   * Someone left the group.
   */
  GROUP_MEMBERSHIP_LEFT             = 0x24 & AL_SYSTEM,

  CREDENTIAL_DELETED                = 0x30 & AL_SYSTEM,
  CREDENTIAL_CREATED                = 0x31 & AL_SYSTEM,
  CREDENTIAL_UPDATED                = 0x32 & AL_SYSTEM,

  SCHEDULE_CREATED                  = 0x40 & AL_SYSTEM,
  SCHEDULE_DELETED                  = 0x41 & AL_SYSTEM,
  SCHEDULE_UPDATED                  = 0x42 & AL_SYSTEM,

  DOOR_CREATED                      = 0x50 & AL_SYSTEM,
  DOOR_UPDATED                      = 0x51 & AL_SYSTEM,
  DOOR_DELETED                      = 0x52 & AL_SYSTEM,
  /**
   * This event is linked to door. It is generated
   * by the Schedule CRUD manager. It's used to let a door
   * know that it may have been removed from a mapping.
   *
   * Fixme: Currently this event is triggered for every door in a mapping
   * everytime a schedule is updated.
   */
  MAPPING_MAY_HAVE_CHANGED          = 0x53 & AL_SYSTEM,

  ACCESS_POINT_CREATED              = 0x60 & AL_SYSTEM,
  ACCESS_POINT_UPDATED              = 0x61 & AL_SYSTEM,
  ACCESS_POINT_DELETED              = 0x62 & AL_SYSTEM,

  ZONE_CREATED                      = 0x70 & AL_SYSTEM,
  ZONE_UPDATED                      = 0x71 & AL_SYSTEM,
  ZONE_DELETED                      = 0x72 & AL_SYSTEM,

  UPDATE_CREATED                    = 0x80 & AL_SYSTEM,
  UPDATE_ACKED                      = 0x81 & AL_SYSTEM,
  UPDATE_CANCELLED                  = 0x81 & AL_SYSTEM,

  DOOR_OPENED                       = 0x10 & AL_USER,
  DOOR_OPENED_MANUALLY              = 0x11 & AL_USER,
  DOOR_FORCED                       = 0x12 & AL_USER,
  DOOR_FORCED_END                   = 0x13 & AL_USER,

  AUTH_GRANTED                      = 0x20 & AL_USER,
  AUTH_DENIED                       = 0x21 & AL_USER,

  LAST__                            = 0xffff
};

using EventMask = FlagSet<EventType>;
}
}
