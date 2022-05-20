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
#include <memory>
#include <odb/lazy-ptr.hxx>

namespace Leosac
{
namespace Alarms
{
using AlarmEntryId = unsigned long;

class Alarm;
using AlarmPtr  = std::shared_ptr<Alarm>;

/**
 * Various Alarm interface forward declaration.
 */

class IAlarmEntry;
using IAlarmEntryPtr = std::shared_ptr<IAlarmEntry>;

/**
 * Forward declaration of implementation class.
 */

class AlarmEntry;
using AlarmEntryLPtr = odb::lazy_shared_ptr<AlarmEntry>;
using AlarmEntryPtr  = std::shared_ptr<AlarmEntry>;
using AlarmEntryWPtr = std::weak_ptr<AlarmEntry>;

enum class AlarmState : uint8_t
{
  STATE_DEFAULT             = 0,
  STATE_RAISED              = 1,
  STATE_DISARMED            = 2,
  STATE_RESOLVED            = 3
};

enum class AlarmSeverity : uint8_t
{
  SEVERITY_LOWEST           = 0,
  SEVERITY_LOW              = 1,
  SEVERITY_NORMAL           = 2,
  SEVERITY_IMPORTANT        = 3,
  SEVERITY_CRITICAL         = 4
};

enum class AlarmType : uint8_t
{
  ALARM_FORCED              = 0,
  ALARM_UNAUTHORIZED_ACCESS = 1,
  ALARM_OFFLINE             = 2,
  ALARM_BATTERY_LOW         = 3,
  ALARM_BATTERY_ACTIVATED   = 4,
  ALARM_SUSPICIOUS_BEHAVIOR = 5
};
}
}
