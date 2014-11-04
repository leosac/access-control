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
                TestAndResetModule(zmqpp::context &ctx, zmqpp::socket *pipe, const boost::property_tree::ptree &cfg);

                virtual ~TestAndResetModule();

            private:
                void process_config();

                void handle_bus_msg();

                /**
                * Do some stuff to let the user known the test card was read.
                */
                void test_sequence();

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
                Hardware::FLED *test_led_;

                /**
                * Buzzer device for test card
                */
                Hardware::FLED *test_buzzer_;
            };

        }
    }
}
