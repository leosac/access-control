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

#include "LedBuzzerSM.hpp"
#include "hardware/facades/FGPIO.hpp"
#include "tools/log.hpp"
#include <chrono>
#include <zmqpp/zmqpp.hpp>

namespace Leosac
{
namespace Module
{
namespace LedBuzzer
{
/**
* Implementation class, for use by the LED module only.
*/
class LedBuzzerImpl
{
  public:
    /**
    * @param ctx ZMQ context
    * @param led_name name of the led object
    * @param gpio_name name of the gpio we use to drive this led.
    */
    LedBuzzerImpl(zmqpp::context &ctx, const std::string &led_name,
                  const std::string &gpio_name, int blink_duration, int blink_speed);

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
    * Only use case is blinking which simplifies code (`ON` with delay is fully
    * handled by GPIO).
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
    * Start blinking, this stores the blink_end timepoint and send commands for
    * blinking
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
    * While we send command directly most of the time (through the backend socket),
    * this can be used too.
    */
    Hardware::FGPIO gpio_;

    int64_t default_blink_duration_;
    int64_t default_blink_speed_;

    /**
    * Our state machine that handle blinking, blinking
    * in pattern or doing nothing.
    */
    LedBuzzerSM stmachine_;
};
}
}
}
