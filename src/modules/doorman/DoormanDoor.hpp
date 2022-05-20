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

#include "core/auth/Auth.hpp"
#include "core/auth/AuthFwd.hpp"
#include "core/alarms/AlarmFwd.hpp"
#include <map>
#include <zmqpp/zmqpp.hpp>

namespace Leosac
{
namespace Module
{
namespace Doorman
{
/**
* Implements a Doorman Door, that is, a component that will listen to door specific event
* unrelated to authentication and react accordingly.
*/
class DoormanDoor
{
  public:
    /**
    * Create a new doorman door.
    *
    * @param door The door that we wish to watch events for
    * @param ctx ZeroMQ context
    * @param actions list of action to do when an event
    */
    DoormanDoor(const Auth::AuthTargetPtr &door, zmqpp::context &ctx);

    DoormanDoor(const DoormanDoor &) = delete;

    DoormanDoor &operator=(const DoormanDoor &) = delete;

    zmqpp::socket &bus_sub();

    /**
    * Activity we care about happened on the bus.
    */
    void handle_bus_msg();

    Leosac::Auth::AuthTargetPtr door() const;

    void alarm_door_forced(Leosac::Alarms::AlarmPtr alarm);

    Leosac::Alarms::AlarmPtr alarm_door_forced() const;

    bool contact_triggered() const;

    std::chrono::system_clock::time_point contact_lastupdate() const;


  private:

    Leosac::Auth::AuthTargetPtr door_;

    zmqpp::socket bus_sub_;

    std::string topic_exitreq_;

    std::string topic_contact_;

    bool contact_triggered_;

    std::chrono::system_clock::time_point contact_lastupdate_;

    Leosac::Alarms::AlarmPtr alarm_door_forced_;
};
}
}
}
