/*
    Copyright (C) 2014-2016 Leosac

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

#include "SysFsGpioModule.hpp"
#include <zmqpp/zmqpp.hpp>

namespace Leosac
{
namespace Module
{
namespace SysFsGpio
{
class SysFsGpioModule;
class SysFsGpioConfig;

/**
* This is a implementation class. It's not exposed to the user and is for this
* module internal code only.
*
* It abstract a GPIO pin.
*/
class SysFsGpioPin
{
  public:
    enum class Direction
    {
        In = 0,
        Out
    };

    enum class InterruptMode
    {
        None,
        Rising,
        Falling,
        Both,
    };

    SysFsGpioPin(zmqpp::context &ctx, const std::string &name, int gpio_no,
                 Direction direction, InterruptMode interrupt_mode,
                 bool initial_value, SysFsGpioModule &module);

    ~SysFsGpioPin();

    SysFsGpioPin(const SysFsGpioPin &) = delete;

    SysFsGpioPin &operator=(const SysFsGpioPin &) = delete;

    SysFsGpioPin &operator=(SysFsGpioPin &&) = delete;

    SysFsGpioPin(SysFsGpioPin &&o) = delete;

    /**
    * Register own socket to the module's reactor.
    * @param reactor Reactor object owned by the module.
    */
    void register_sockets(zmqpp::reactor *reactor);

    /**
    * This method shall returns the time point at which we want to be updated.
    */
    std::chrono::system_clock::time_point next_update() const;

    /**
     * Update the PIN.
     *
     * The update will simply turn the PIN off (as a timeout for `ON` command).
     *
     * @note This is similar to PFDigitalPin.
     */
    void update();

  private:
    /**
    * Interrupt happened for this GPIO ping.
    */
    void handle_interrupt();

    /**
    * Read value from filesystem.
    */
    bool read_value();

    /**
    * Write to sysfs to turn the gpio on.
    */
    bool turn_on(zmqpp::message *msg = nullptr);

    /**
    * Write to sysfs to turn the gpio on.
    */
    bool turn_off();

    /**
    * Read to sysfs and then write the opposite value
    */
    bool toggle();

    /**
    * The SysFsGpioModule will register this method so its called when a message
    * is ready on the pin socket.
    */
    void handle_message();

    /**
    * Write direction to the `direction` file.
    */
    void set_direction(Direction dir);

    /**
    * Write interrupt mode to the `edge` file.
    */
    void set_interrupt(InterruptMode mode);

    /**
    * File descriptor of the GPIO in sysfs.
    */
    int file_fd_;

    /**
    * Number of the GPIO.
    */
    int gpio_no_;

    /**
    * listen to command from other component.
    */
    zmqpp::socket sock_;

    std::string name_;

    /**
    * Direction of the PIN.
    */
    const Direction direction_;

    /**
    * Initial value of the PIN. We set the pin's value to this on module shutdown.
    */
    const bool initial_value_;

    /**
    * Reference to the module. We use this to publish on the bus.
    */
    SysFsGpioModule &module_;

    const SysFsGpioConfig &path_cfg_;

    /**
    * Time point of next wished update. (Used for timeout on `ON`)
    */
    std::chrono::system_clock::time_point next_update_time_;
};
}
}
}