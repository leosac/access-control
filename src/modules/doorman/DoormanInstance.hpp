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
#include "DoormanDoor.hpp"
#include <map>
#include <zmqpp/zmqpp.hpp>

namespace Leosac
{
namespace Module
{
namespace Doorman
{
class DoormanModule;
/**
* Helper struct to wrap an "action".
* It is composed of a `target_` (object name), a field (`on_`) to know when the
* action should take place, and a list of string that represents the command.
*/
struct DoormanAction
{
    /**
    * Target component. Will be reach through REQ.
    */
    std::string target_;

    /**
    * When should this action be done? on GRANTED or DENIED ?
    */
    Leosac::Auth::AccessStatus on_;

    /**
    * The command to be send. Each element of the vector will be a frame in the
    * zmqpp::message.
    */
    std::vector<std::string> cmd_;
};

/**
* Implements a Doorman, that is, a component that will listen to authentication event
* and react accordingly.
* The reaction is somehow scriptable through the configuration file.
*/
class DoormanInstance
{
  public:
    /**
    * Create a new doorman.
    *
    * @param ctx ZeroMQ context
    * @param name the name of this doorman
    * @param auth_contexts list of authentication context (by name) that we wish to
    * watch
    * @param actions list of action to do when an event
    */
    DoormanInstance(DoormanModule &module, zmqpp::context &ctx,
                    const std::string &name,
                    const std::vector<std::string> &auth_contexts,
                    const std::vector<DoormanAction> &actions);

    DoormanInstance(const DoormanInstance &) = delete;

    DoormanInstance &operator=(const DoormanInstance &) = delete;

    zmqpp::socket &bus_sub();

    /**
    * Activity we care about happened on the bus.
    */
    void handle_bus_msg();

    const std::vector<std::shared_ptr<DoormanDoor>> &doors() const;

  private:
    /**
    * Should we ignore this action.
    *
    * There are multiple reason why we might wanna ignore an action:
    *    1. The expected status (`granted` / `denied`) does not match the received
    * status.
    *    2. The door is in always_open (or alway_closed) mode.
    */
    bool ignore_action(const DoormanAction &action, Auth::AccessStatus status) const;

    Auth::AuthTargetPtr find_target(const std::string &name) const;

    std::vector<std::shared_ptr<DoormanDoor>> doors_;

    /**
    * Send a command to a target and wait for response.
    *
    * @param target_name name of target object
    * @param msg message containing command (and command parameter) to send
    */
    void command_send_recv(const std::string &target_name, zmqpp::message msg);

    std::string name_;

    std::vector<DoormanAction> actions_;

    zmqpp::socket bus_sub_;

    /**
    * Socket (REP) connected to each target this doorman may have
    */
    std::map<std::string, zmqpp::socket> targets_;
};
}
}
}
