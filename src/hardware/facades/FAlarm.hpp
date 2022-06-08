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

#include <string>
#include <zmqpp/zmqpp.hpp>

namespace Leosac
{
namespace Hardware
{

enum class AlarmState : uint8_t
{
  STATE_DEFAULT             = 0,
  STATE_RAISED              = 1,
  STATE_DISARMED            = 2,
  STATE_RESOLVED            = 3,
  STATE_UNKNOWN             = 255
};

enum class AlarmType : uint8_t
{
  ALARM_UNKNOWN             = 0,
  ALARM_FORCED              = 1,
  ALARM_UNAUTHORIZED_ACCESS = 2,
  ALARM_OFFLINE             = 3,
  ALARM_BATTERY_LOW         = 4,
  ALARM_BATTERY_ACTIVATED   = 5,
  ALARM_SUSPICIOUS_BEHAVIOR = 6
};

/**
* Facade object for an Alarm.
*/
class FAlarm
{
  public:

    /**
    * Construct a facade to an alarm.
    * @param ctx ZMQ context
    * @param name name of the alarm
    */
    FAlarm(zmqpp::context &ctx, const std::string &name);

    FAlarm(const FAlarm &) = delete;

    FAlarm &operator=(const FAlarm &) = delete;

    ~FAlarm() = default;

    /**
     * Raise the alarm.
     */
    std::string raise(AlarmType type, const std::string& reason);

    /**
     * Get the current alarm state.
     */
    AlarmState state(const std::string& alarm);

    /**
     * Set the alarm state.
     */
    bool state(const std::string& alarm, AlarmState state);

    /**
     * Disarm the alarm.
     */
    bool disarm(const std::string& alarm);

    /**
    * Returns the alarm's name.
    */
    const std::string &name() const;

  private:

    /**
    * Send a message to the `backend_` alarm and wait for a response.
    */
    zmqpp::message_t send_to_backend(zmqpp::message &m);

    /**
    * A socket to talk to the backend alarm.
    */
    zmqpp::socket backend_;

    std::string name_;
};
}
}
