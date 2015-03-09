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

#include <string>
#include <chrono>
#include <zmqpp/socket.hpp>

namespace Leosac
{
    namespace Hardware
    {

        /**
        * A Facade to a GPIO object.
        * This facade does not care about the GPIO implementation. It uses message passing to interact
        * with the application "gpio controller".
        *
        * The backend GPIO object (implemented by the gpio module (either sysfs or piface)) MUST exist.
        * All you need is the GPIO name defined in the configuration file to create a facade.
        *
        * @note This class implements the client code to [theses specifications](@ref hardware_spec_gpio).
        */
        class FGPIO
        {
        public:
            FGPIO(zmqpp::context &ctx, const std::string &gpio_name);

            /**
            * Disabled copy-constructor.
            * Manually create a new facade using the GPIO's name instead.
            */
            FGPIO(const FGPIO &) = delete;

            /**
            * Default destructor, RAII does the job.
            */
            ~FGPIO() = default;

            /**
            * Turn the GPIO ON and turn it OFF duration milliseconds later.
            */
            bool turnOn(std::chrono::milliseconds duration);

            /**
            * Turn the GPIO ON by sending a message to the backend GPIO impl.
            */
            bool turnOn();

            /**
            * Turn the GPIO OFF by sending a message to the backend GPIO impl.
            */
            bool turnOff();

            /**
            * Toggle the GPIO value by sending a message to the backend GPIO impl.
            */
            bool toggle();

            /**
            * Query the value of the GPIO and returns true if the LED is ON.
            * It returns false otherwise.
            */
            bool isOn();

            /**
            * Similar to `isOn()`.
            */
            bool isOff();

            /**
            * Name of the GPIO pin as defined in the configuration file.
            */
            const std::string &name() const;

        private:
            std::string gpio_name_;

            /**
            * A socket to talk to the backend GPIO.
            */
            zmqpp::socket backend_;
        };

    }
}
