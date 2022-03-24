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

#include "core/auth/Auth.hpp"
#include "hardware/facades/FBuzzer.hpp"
#include "hardware/facades/FLED.hpp"
#include "modules/wiegand/strategies/WiegandStrategy.hpp"
#include "zmqpp/zmqpp.hpp"
#include <chrono>
#include <string>

namespace Leosac
{
namespace Module
{

namespace Wiegand
{
/**
* An implementation class that represents a Wiegand Reader.
* It's solely for internal use by the Wiegand module.
*/
class WiegandReaderImpl
{
  public:
    /**
    * Create a new implementation of a Wiegand Reader.
    * @param ctx ZMQ context.
    * @param data_high_pin name of the GPIO connected to data high.
    * @param data_low_pin name of the GPIO connected to data low.
    * @param green_led_name name of the "green led" LED device.
    * @param buzzer_name name of the buzzer device. -- no buzzer module yet.
    * @param strategy strategy (mode implementation) the reader is using
    */
    WiegandReaderImpl(zmqpp::context &ctx, const std::string &reader_name,
                      const std::string &data_high_pin,
                      const std::string &data_low_pin,
                      const std::string &green_led_name,
                      const std::string &buzzer_name,
                      std::unique_ptr<Strategy::WiegandStrategy> strategy);

    ~WiegandReaderImpl();

    WiegandReaderImpl(const WiegandReaderImpl &) = delete;

    WiegandReaderImpl &operator=(const WiegandReaderImpl &) = delete;

    WiegandReaderImpl(WiegandReaderImpl &&o);

    /**
    * Socket that allows the reader to listen to the application BUS.
    */
    zmqpp::socket bus_sub_;

    /**
    * REP socket to receive command on.
    */
    zmqpp::socket sock_;

    /**
    * Something happened on the bus.
    */
    void handle_bus_msg();

    /**
    * Someone sent a request.
    */
    void handle_request();

    /**
    * Timeout (no more data burst to handle). The WiegandModule call this when
    * polling on any wiegand reader times out.
    * The reader shall publish an event if it received any meaningful message since
    * the last timeout.
    */
    void timeout();

    /**
    * Reset the "read state" of the reader, effectively cleaning the
    * wiegand-bit-buffer
    * and resetting the counter to 0.
    */
    void read_reset();

    /**
    * Returns the number of bits read.
    * This number of bits shall never be greater than the number of bits the buffer_
    * can hold.
    */
    int counter() const;

    /**
    * Return a pointer to internal buffer memory. You can use this
    * to access the bits read by the reader.
    * Do not read more bits than counter() returned.
    */
    const unsigned char *buffer() const;

    /**
    * Returns the name of this reader.
    */
    const std::string &name() const;

  private:
    /**
    * Socket to write to the message bus.
    */
    zmqpp::socket bus_push_;

    /**
    * ZMQ topic-string for interrupt on HIGH gpio (high gpio's name)
    */
    std::string topic_high_;

    /**
    * ZMQ topic-string to interrupt on LOW gpio (low gpio's name)
    */
    std::string topic_low_;

    /**
    * Buffer to store incoming bits from high and low gpios.
    */
    std::array<uint8_t, 16> buffer_;

    /**
    * Count the number of bits received from GPIOs.
    */
    int counter_;

    /**
    * Name of the device (defined in configuration)
    */
    std::string name_;

    /**
    * Facade to control the reader green led.
    */
    std::unique_ptr<Hardware::FLED> green_led_;

    /**
    * Facade to the buzzer object
    */
    std::unique_ptr<Hardware::FBuzzer> buzzer_;

    /**
    * Concrete implementation of the reader mode.
    */
    std::unique_ptr<Strategy::WiegandStrategy> strategy_;
};
}
}
}
