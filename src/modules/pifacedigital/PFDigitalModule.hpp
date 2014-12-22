#pragma once

#include <zmqpp/socket.hpp>
#include <boost/property_tree/ptree.hpp>
#include <zmqpp/reactor.hpp>
#include <modules/BaseModule.hpp>
#include "PFDigitalPin.hpp"

namespace Leosac
{
    namespace Module
    {
        /**
        * Provide support for the piface digital device.
        *
        * @see @ref mod_piface_main for documentation
        */
        namespace Piface
        {
            /**
            * Main class for the piface digital module.
            */
            class PFDigitalModule : public BaseModule
            {
            public:
                PFDigitalModule(const boost::property_tree::ptree &config,
                        zmqpp::socket *module_manager_pipe,
                        zmqpp::context &ctx);

                /**
                * Module's main loop.
                */
                virtual void run() override;

            private:

                /**
                * An interrupt was triggered. Lets handle it.
                */
                void handle_interrupt();

                /**
                * Process the configuration, preparing configured GPIO pin.
                */
                void process_config(const boost::property_tree::ptree &cfg);

                /**
                * Compute the poll timeout (in milliseconds) and returns it.
                * This timeout is calculated by calling `next_update()` on the available GPIO.
                */
                int compute_timeout();

                /**
                * Socket to push event to the bus.
                */
                zmqpp::socket bus_push_;

                /**
                * GPIO vector
                */
                std::vector<PFDigitalPin> gpios_;

                /**
                * Should be removed someday...
                * store the name of the input pin with id = idx in dest.
                *
                * returns true if it was succesful (pin exists), false otherwise.
                */
                bool get_input_pin_name(std::string &dest, int idx);

                /**
                * File descriptor of the PIN that triggers interrupts. This is card and will not change.
                */
                int interrupt_fd_;
            };

        }
    }
}
