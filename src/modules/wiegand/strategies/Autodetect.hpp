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

#include "Strategies.hpp"

namespace Leosac
{
namespace Module
{

namespace Wiegand
{
namespace Strategy
{
/**
* Variable strategy that tries to autodetect what it reads.
* It can credentials multiple type of credentials, based on what was detected.
*
* We have 3 possible scenario:
*     + User only swipes their card.
*     + User swipes their card, and then input a PIN code.
*     + User input a PIN code.
*/
class Autodetect : public WiegandStrategy
{
  public:
    /**
    * Create a strategy that read whatever it can and tries its best
    * to determine what is was that it read.
    *
    * @param reader         the reader object we provide the strategy for.
    * @param delay          max nb of msec between reading the card and receiving pin
    * code data.
    * @param pin_key_end    what key on the reader signals the end of the pin code ?
    */
    Autodetect(WiegandReaderImpl *reader, std::chrono::milliseconds delay,
               char pin_key_end, bool nowait);

    virtual void timeout() override;

    virtual bool completed() const override;

    virtual void signal(zmqpp::socket &sock) override;

    virtual void set_reader(WiegandReaderImpl *new_ptr) override;

  private:
    /**
    * Reset self.
    * We create new strategy instance, reset the boolean flag to defaults and
    * reset the underlying reader buffer.
    */
    void reset() override;

    /**
    * Dynamically instanciate a new strategy based on the number of bits
    * available.
    *
    * This depends on the wiegand reader mode, and this class support either 4 or 8
    * bits.
    *
    * @return a new strategy to handle 4 or 8 bits mode.
    * @note Assert if !(bits == 4 || bits == 8);
    */
    PinReadingUPtr build_strategy(int bits);

    /**
    * Called when `timeout()` was called but nothing was read.
    */
    void check_timeout();

    CardReadingUPtr read_card_strategy_;
    PinReadingUPtr read_pin_strategy_;

    std::chrono::milliseconds delay_;
    using TimePoint = std::chrono::system_clock::time_point;
    TimePoint time_card_read_;
    TimePoint last_pin_read_;

    bool reading_pin_;
    bool reading_card_;
    bool ready_;

    char pin_key_end_;
    bool nowait_;
};
}
}
}
}
