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

#include "modules/BaseModule.hpp"
#include "hardware/FLED.hpp"

namespace Leosac
{
    namespace Module
    {
        /**
        * This module provide a way to have feedback when presenting a test card.
        * It also allow to reset Leosac to its stock configuration.
        *
        * @see @ref mod_testandreset_main
        */
        namespace TestAndReset
        {

            class TestAndResetModule : public BaseModule
            {
            public:
                explicit TestAndResetModule(zmqpp::context &ctx,
                                            zmqpp::socket *pipe,
                                            const boost::property_tree::ptree &cfg,
                                            CoreUtilsPtr  utils);

                TestAndResetModule(const TestAndResetModule &) = delete;
                TestAndResetModule(TestAndResetModule &&) = delete;
                TestAndResetModule &operator=(const TestAndResetModule &) = delete;
                TestAndResetModule &operator=(TestAndResetModule &&) = delete;

                virtual ~TestAndResetModule();

            private:
                void process_config();
                
                /**
                * Do some stuff to let the user known something happened.
                * This may be trigger on module initialization, and will be called when
                * test card is read.
                */
                void run_test_sequence();

                void handle_bus_msg();

                /**
                * Do some stuff to let the user known the test card was read.
                */
                void test_sequence();

                /**
                * Search the device_reset_card_ map for an entry whose value
                * is card_id;
                */
                bool has_reset_card(const std::string &card_id) const;

                /**
                * REQ socket to kernel
                */
                zmqpp::socket kernel_sock_;

                /**
                * Sub socket on the BUS
                */
                zmqpp::socket sub_;

                /**
                * Map a device name to the reset card, since one device can support one reset card currently.
                */
                std::map<std::string, std::string> device_reset_card_;

                /**
                *  Map a device name to the test card.
                */
                std::map<std::string, std::string> device_test_card_;

                /**
                * Led device for test card
                */
                std::unique_ptr<Hardware::FLED> test_led_;

                /**
                * Buzzer device for test card
                */
                std::unique_ptr<Hardware::FLED> test_buzzer_;

                /**
                * Play the sequence on module startup.
                */
                bool run_on_start_;

                bool promisc_;
            };

        }
    }
}
