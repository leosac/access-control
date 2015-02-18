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
                * Strategy for PIN only, 8 bits per key pressed mode.
                */
                class WiegandPin8BitsOnly : public PinReading
                {
                public:
                    /**
                    * Create a strategy that read 4bits-per-key PIN code.
                    *
                    * @param reader         the reader object we provide the strategy for.
                    * @param pin_timeout    nb of msec before flushing user input to the system
                    * @param pin_end_key    key ('1', '*', '5') that will trigger user input flushing.
                    */
                    WiegandPin8BitsOnly(WiegandReaderImpl *reader,
                            std::chrono::milliseconds pin_timeout,
                            char pin_end_key);

                    // we reset the counter_ and buffer_ for each key.
                    virtual void timeout() override;

                    virtual bool completed() const override;

                    virtual void signal(zmqpp::socket &sock) override;

                    virtual const std::string &get_pin() const override;

                    virtual void reset() override;

                private:

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
