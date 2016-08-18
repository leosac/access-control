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

#include <chrono>
#include <string>
#include <zmqpp/poller.hpp>
#include <zmqpp/socket.hpp>

namespace Leosac
{
namespace Hardware
{

/**
* A Facade to a LED object.
*
* This object require a valid GPIO object to work. It abstract a LED, and add a
* simple command on top of GPIO.
* It requires a module that implements the behavior to work properly. However, unlike
* GPIO modules, we are unlikely
* to have multiple implementation of a LED module.
*
* @note This class implements the client code to [theses specifications](@ref
* hardware_spec_led).
*/
class FLED
{
  public:
    struct State
    {
        State()
            : st(UNKNOWN)
            , duration(0)
            , speed(0)
            , value(false)
        {
        }

        /**
        * Internal state of the LED.
        */
        enum
        {
            ON,
            OFF,
            BLINKING,
            UNKNOWN,
        } st;

        /**
        * Set only if `st` is `BLINKING`, it represents the total duration of
        * blinking.
        */
        int64_t duration;

        /**
        * Set only if `st` is `BLINKING`, it represents the speed of blinking.
        */
        int64_t speed;

        /**
        * Set only if `st` is `BLINKING` : value of the LED (true if ON, false
        * otherwise).
        */
        bool value;
    };

    FLED(zmqpp::context &ctx, const std::string &led_name);

    /**
    * Disabled copy-constructor.
    * Manually create a new facade using the LED's name instead.
    */
    FLED(const FLED &) = delete;

    /**
    * Default destructor
    */
    ~FLED() = default;

    /**
    * Turn the LED ON and turn it OFF duration milliseconds later.
    */
    bool turnOn(int duration);

    /**
    * Turn the LED ON and turn it OFF duration milliseconds later.
    */
    bool turnOn(std::chrono::milliseconds duration);

    /**
    * Turn the LED ON by sending a message to the backend LED impl.
    */
    bool turnOn();

    /**
    * Turn the LED OFF by sending a message to the backend LED impl.
    */
    bool turnOff();

    /**
    * Toggle the LED value by sending a message to the backend LED impl.
    */
    bool toggle();

    /**
    * Make the LED blink. No optional parameter so the module shall use the default
    * for the device.
    */
    bool blink();

    /**
    * Blink with a duration and a speed.
    */
    bool blink(std::chrono::milliseconds duration, std::chrono::milliseconds speed);

    bool blink(int duration, int speed);

    /**
    * Query the value of the GPIO and returns true if the LED is ON.
    * It returns false otherwise.
    *
    * If the GPIO is blinking, but currently ON, this returns true.
    */
    bool isOn();

    /**
    * Similar to `isOn()`.
    *
    * If the GPIO is blinking, but currently OFF, this returns true.
    */
    bool isOff();

    /**
    * Returns true is the LED is currently blinking.
    */
    bool isBlinking();

    /**
    * Return the state of the device.
    * See FLED::State for more infos.
    */
    State state();

    /**
    * Access the backend socket (which is connect to the LED device) to send command
    * directly.
    * Use carefully !
    */
    zmqpp::socket &backend();

  private:
    /**
    * A socket to talk to the backend LED.
    */
    zmqpp::socket backend_;

    /**
     * A poller to not wait for infinity in case something went wrong.
     */
    zmqpp::poller poller_;
};
}
}
