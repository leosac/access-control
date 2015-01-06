/*
    Copyright (C) 2014-2015 Islog

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

#include <map>
#include <zmqpp/zmqpp.hpp>

namespace Leosac
{
    namespace Module
    {
        namespace Doorman
        {
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
                * The command to be send. Each element of the vector will be a frame in the zmqpp::message.
                */
                std::vector<std::string> cmd_;
            };

            /**
            * Implements a Doorman, that is, a component that will listen to authentication event and react accordingly.
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
                * @param auth_contexts list of authentication context (by name) that we wish to watch
                * @param actions list of action to do when an event
                * @param timeout see timeout field description.
                */
                DoormanInstance(zmqpp::context &ctx,
                        const std::string &name,
                        const std::vector<std::string> &auth_contexts,
                        const std::vector<DoormanAction> &actions,
                        int timeout);

                DoormanInstance(const DoormanInstance &) = delete;

                DoormanInstance &operator=(const DoormanInstance &) = delete;

                zmqpp::socket &bus_sub();

                /**
                * Activity we care about happened on the bus.
                */
                void handle_bus_msg();

            private:

                /**
                * Send a command to a target and wait for response.
                *
                * @param target_name name of target object
                * @param msg message containing command (and command parameter) to send
                */
                void command_send_recv(const std::string &target_name, zmqpp::message msg);

                std::string name_;

                std::vector<DoormanAction> actions_;

                /**
                * This timeout value is used to determine if 2 auth event are accepted "as one".
                * Passed this delay, one event will not have any impact to the other.
                * //fixme rephrase that.
                */
                int timeout_;

                zmqpp::socket bus_sub_;

                /**
                * Socket (REP) connected to each target this doorman may have
                */
                std::map<std::string, zmqpp::socket> targets_;
            };
        }
    }
}
