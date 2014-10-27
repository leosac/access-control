#pragma once

#include <zmqpp/zmqpp.hpp>
#include <chrono>
#include "hardware/device/FGPIO.hpp"

namespace Leosac
{
    namespace Module
    {
        namespace Led
        {
            /**
            * Implementation class, for use by the LED module only.
            */
            class LedImpl
            {
            public:
                /**
                * @param ctx ZMQ context
                * @param led_name name of the led object
                * @param gpio_name name of the gpio we use to drive this led.
                */
                LedImpl(zmqpp::context &ctx,
                        const std::string &led_name,
                        const std::string &gpio_name,
                        int blink_duration,
                        int blink_speed);

                /**
                * Return the `frontend_` socket.
                */
                zmqpp::socket &frontend();

                /**
                * Message received on the `rep_` socket.
                */
                void handle_message();

                /**
                * Time point of the next wanted update.
                * Set to time_point::max() if not wanted.
                */
                std::chrono::system_clock::time_point next_update();

                /**
                * Update the object.
                * Only use case is blinking which simplifies code (`ON` with delay is fully handled by GPIO).
                */
                void update();

            private:
                /**
                * Send a message to the backend object (used for ON, OFF, TOGGLE).
                * Return the response message.
                */
                zmqpp::message send_to_backend(zmqpp::message &msg);

                /**
                * Write the current state of the LED device (according to specs)
                * to the `frontend_` socket.
                */
                void send_state();

                /**
                * Start blinking, this stores the blink_end timepoint and send commands for blinking
                * to happen. Register `update()`.
                */
                bool start_blink(zmqpp::message *msg);

                zmqpp::context &ctx_;

                /**
                * REP socket to receive LED command.
                */
                zmqpp::socket frontend_;

                /**
                * REQ socket to the backend GPIO.
                */
                zmqpp::socket backend_;

                /**
                * Facade to the GPIO we use with this LED.
                * While we send command directly most of the time (through the backend socket), this can be used too.
                */
                FGPIO gpio_;

                /**
                * Does this LED want its `update()` method be called by the LEDModule.
                */
                bool want_update_;

                int default_blink_duration_;
                int default_blink_speed_;

                int blink_speed_;
                int blink_duration_;

                /**
                * Number of toggle left to do. This starts at N and decrements over time. When it reaches 0 it means
                * we can stop toggling the gpio.
                */
                int blink_count_;

                std::chrono::system_clock::time_point next_update_time_;
            };

        }
    }
}