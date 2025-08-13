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
  // 0..63 reserved to keep masks within 64-bit numeric serialization

  // 0..15 System/Admin
  WSAPI_CALL                        = 0,
  USER_CREATED                      = 1,
  USER_DELETED                      = 2,
  USER_EDITED                       = 3,
  USER_PASSWORD_CHANGED             = 4,
  USER_PASSWORD_CHANGE_FAILURE      = 5,

  // 16..23 Groups
  GROUP_CREATED                     = 16,
  GROUP_UPDATED                     = 17,
  GROUP_DELETED                     = 18,
  GROUP_MEMBERSHIP_JOINED           = 19,
  GROUP_MEMBERSHIP_LEFT             = 20,

  // 24..27 Credentials
  CREDENTIAL_CREATED                = 24,
  CREDENTIAL_UPDATED                = 25,
  CREDENTIAL_DELETED                = 26,

  // 28..30 Schedules
  SCHEDULE_CREATED                  = 28,
  SCHEDULE_UPDATED                  = 29,
  SCHEDULE_DELETED                  = 30,

  // 31..35 Doors
  DOOR_CREATED                      = 31,
  DOOR_UPDATED                      = 32,
  DOOR_DELETED                      = 33,
  MAPPING_MAY_HAVE_CHANGED          = 34,

  // 36..38 Access points
  ACCESS_POINT_CREATED              = 36,
  ACCESS_POINT_UPDATED              = 37,
  ACCESS_POINT_DELETED              = 38,

  // 39..41 Zones
  ZONE_CREATED                      = 39,
  ZONE_UPDATED                      = 40,
  ZONE_DELETED                      = 41,

  // 42..44 Updates
  UPDATE_CREATED                    = 42,
  UPDATE_ACKED                      = 43,
  UPDATE_CANCELLED                  = 44,

  // 45..48 Door usage
  DOOR_OPENED                       = 45,
  DOOR_OPENED_MANUALLY              = 46,
  DOOR_FORCED                       = 47,
  DOOR_FORCED_END                   = 48,

  // 49..50 Auth outcomes
  AUTH_GRANTED                      = 49,
  AUTH_DENIED                       = 50,

  LAST__                            = 64
};

using EventMask = FlagSet<EventType>;
}
}
