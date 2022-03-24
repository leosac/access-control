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

#include <zmqpp/zmqpp.hpp>
#include <boost/property_tree/ptree.hpp>
#include <vector>
#include <memory>
#include "modules/BaseModule.hpp"
#include "hardware/facades/FGPIO.hpp"
#include "tools/XmlScheduleLoader.hpp"
#include "core/auth/AuthTarget.hpp"

namespace Leosac
{
    namespace Module
    {
        /**
        * Module that publish wiegand auth event on a ZMQ publisher.
        *
        * @see @ref mod_event_publish_main for end-user documentation.
        */
        namespace EventPublish
        {

            /**
            * Run the Event publication module.
            *
            * @see @ref mod_event_publish_user_config for configuration information.
            */
            class EventPublish : public BaseModule
            {
            public:
                EventPublish(zmqpp::context &ctx,
                        zmqpp::socket *pipe,
                        const boost::property_tree::ptree &cfg,
                        CoreUtilsPtr utils);

                EventPublish(const EventPublish &) = delete;

                EventPublish &operator=(const EventPublish &) = delete;

                ~EventPublish() = default;

            private:
                void handle_msg_bus();

                /**
                * Processing the configuration tree, spawning AuthFileInstance object as described in the
                * configuration file.
                */
                void process_config();

                /**
                 * Read internal message bus.
                 */
                zmqpp::socket bus_sub_;

                /**
                 * Publish to the internet.
                 */
                zmqpp::socket network_pub_;

                bool publish_source_;
            };

        }
    }
}
