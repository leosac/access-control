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

#include "PinReading.hpp"

namespace Leosac
{
namespace Module
{
namespace Wiegand
{
namespace Strategy
{
/**
* Strategy for ready PIN only.
 * The number of bits sent by the reader per key pressed
 * is NbBits.
*/
template <unsigned int NbBits>
class WiegandPinNBitsOnly : public PinReading
{
  public:
    /**
    * Create a strategy that read N bits-per-key PIN code.
    *
    * @param reader         the reader object we provide the strategy for.
    * @param pin_timeout    nb of msec before flushing user input to the system
    * @param pin_end_key    key ('1', '*', '5') that will trigger user input
    * flushing.
    */
    WiegandPinNBitsOnly(WiegandReaderImpl *reader,
                        std::chrono::milliseconds pin_timeout, char pin_end_key);

    // we reset the counter_ and buffer_ for each key.
    virtual void timeout() override;

    virtual bool completed() const override;

    virtual void signal(zmqpp::socket &sock) override;

    virtual const std::string &get_pin() const override;

    virtual void reset() override;

  private:
    /**
     * Extract the character that was pressed from
     * raw data.
     */
    char extract_from_raw(uint8_t input) const;

    /**
    * Timeout or pin_end_key read. If we have meaningful data,
    * set ready to true.
    */
    void end_of_input();

    std::string inputs_;
    std::chrono::milliseconds pin_timeout_;
    char pin_end_key_;

    using TimePoint = std::chrono::system_clock::time_point;
    TimePoint last_update_;

    /**
    * Are we ready to submit the PIN code ?
    */
    bool ready_;
};
}
}
}
}
