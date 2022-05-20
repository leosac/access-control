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

#include "DoormanDoor.hpp"
#include "core/auth/AuthTarget.hpp"
#include "core/auth/Auth.hpp"
#include "tools/log.hpp"

using namespace Leosac::Module::Doorman;

DoormanDoor::DoormanDoor(const Auth::AuthTargetPtr &door, zmqpp::context &ctx)
    : door_(door)
    , bus_sub_(ctx, zmqpp::socket_type::sub)
    , contact_triggered_(false)
{
  bus_sub_.connect("inproc://zmq-bus-pub");

  if (door->exitreq_gpio())
  {
    topic_exitreq_ = "S_INT:" + door->exitreq_gpio()->name();
    bus_sub_.subscribe(topic_exitreq_);
  }
  if (door->contact_gpio())
  {
    topic_contact_ = "S_INT:" + door->contact_gpio()->name();
    bus_sub_.subscribe(topic_contact_);
  }
}

Leosac::Auth::AuthTargetPtr DoormanDoor::door() const
{
  return door_;
}

void DoormanDoor::alarm_door_forced(Leosac::Alarms::AlarmPtr alarm)
{
  alarm_door_forced_ = alarm;
}

Leosac::Alarms::AlarmPtr DoormanDoor::alarm_door_forced() const
{
  return alarm_door_forced_;
}

zmqpp::socket &DoormanDoor::bus_sub()
{
  return bus_sub_;
}

void DoormanDoor::handle_bus_msg()
{
  std::string msg;
  bus_sub_.receive(msg);

  if (msg == topic_exitreq_)
  {
    Hardware::FGPIO *gpio = door_->gpio();
    if (gpio)
    {
      gpio->turnOn(door_->exitreq_duration());
    }
  }
  else if (msg == topic_contact_)
  {
    contact_triggered_ = !contact_triggered_;
    contact_lastupdate_ = std::chrono::system_clock::now();
  }
}

bool DoormanDoor::contact_triggered() const
{
  return contact_triggered_;
}

std::chrono::system_clock::time_point DoormanDoor::contact_lastupdate() const
{
  return contact_lastupdate_;
}
