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

#include <zmqpp/zmqpp.hpp>
#include <boost/property_tree/ptree.hpp>
#include <vector>
#include <memory>
#include <modules/BaseModule.hpp>
#include <hardware/FGPIO.hpp>
#include <tools/XmlScheduleLoader.hpp>
#include <core/auth/AuthTarget.hpp>

namespace Leosac
{
    namespace Module
    {
        /**
        * Module that allows user to configure action to be taken
        * to react to messages from other modules.
        *
        * @see @ref mod_doorman_main for end-user documentation.
        */
        namespace Doorman
        {

            class DoormanInstance;

            class Door
            {
            public:
                //std::list<Tools::XmlScheduleLoader::Schedule> always_on;
                std::unique_ptr<Hardware::FGPIO> gpio_;
            };

            /**
            * Main class for the module, it create handlers and run them
            * to, well, handle events and send command.
            *
            * @see @ref mod_doorman_user_config for configuration information.
            */
            class DoormanModule : public BaseModule
            {
            public:
                DoormanModule(zmqpp::context &ctx,
                        zmqpp::socket *pipe,
                        const boost::property_tree::ptree &cfg);

                DoormanModule(const DoormanModule &) = delete;

                DoormanModule &operator=(const DoormanModule &) = delete;

                ~DoormanModule() = default;

                virtual void run() override;

            private:

                void update();

                /**
                * Processing the configuration tree, spawning AuthFileInstance object as described in the
                * configuration file.
                */
                void process_config();

                void process_doors_config(const boost::property_tree::ptree &t);

                /**
                * Authenticator instances.
                */
                std::vector<std::shared_ptr<DoormanInstance>> doormen_;

                /**
                * Doors, to manage the always-on or always off stuff.
                */
                std::vector<std::shared_ptr<Leosac::Auth::AuthTarget>> doors_;
            };

        }
    }
}
