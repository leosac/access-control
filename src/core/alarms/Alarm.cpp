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

#include "core/alarms/Alarm.hpp"
#include "core/alarms/AlarmEntry.hpp"
#include "core/auth/User.hpp"
#include "core/CoreUtils.hpp"
#include "tools/AssertCast.hpp"
#include "tools/db/OptionalTransaction.hpp"
#include "tools/log.hpp"
#include <zmqpp/zmqpp.hpp>

using namespace Leosac;
using namespace Leosac::Alarms;

Alarm::Alarm(zmqpp::context &ctx, const DBPtr &database, const std::string& name, AlarmType type, AlarmSeverity severity)
  : name_(name)
  , type_(type)
  , severity_(severity)
  , bus_push_(ctx, zmqpp::socket_type::push)
  , database_(database)
{
  bus_push_.connect("inproc://zmq-bus-pull");
}

void Alarm::raise(const std::string& reason, Auth::UserPtr user)
{
  author_ = user;
  reason_ = reason;

  if (database_)
  {
    last_entry_ = AlarmEntry::create(database_, name_, type_, severity_, reason_);
    last_entry_->author(author_);
  }

  changeState(AlarmState::STATE_RAISED);
}

void Alarm::disarm()
{
  ASSERT_LOG(state_ == AlarmState::STATE_RAISED, "The alarm wasn't raised.");

  changeState(AlarmState::STATE_DISARMED);
}

void Alarm::changeState(AlarmState state)
{
  INFO("Alarm {type: " << static_cast<uint8_t>(type_)
        << " - severity: " << static_cast<uint8_t>(severity_)
        << "  - reason: " << reason_
        << "} changed state from " << static_cast<uint8_t>(state) << " to " << static_cast<uint8_t>(state_));

  state_ = state;
  if (database_ && last_entry_)
  {
    db::OptionalTransaction t(database_->begin());
    last_entry_->state(state_);
    t.commit();
  }
  sendmsg();
}

void Alarm::sendmsg()
{
  if (!name_.empty())
  {
    zmqpp::message msg;
    msg << std::string("S_" + name_) << static_cast<uint8_t>(severity_) << static_cast<uint8_t>(type_) << static_cast<uint8_t>(state_) << reason_;
    bus_push_.send(msg);
  }
}
