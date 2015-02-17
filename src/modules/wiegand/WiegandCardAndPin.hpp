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

#include "WiegandStrategy.hpp"

namespace Leosac
{
    namespace Module
    {

        namespace Wiegand
        {
            /**
            * Strategy for reading a card then a PIN code.
            * We reuse existing strategy.
            */
            class WiegandCardAndPin : public WiegandStrategy
            {
            public:
                /**
                * Create a strategy that read 4bits-per-key PIN code.
                *
                * @param reader         the reader object we provide the strategy for.
                * @param delay          max nb of msec between reading the card and receiving pin code data.
                * @param pin_timeout    nb of msec before flushing user input to the system
                * @param pin_end_key    key ('1', '*', '5') that will trigger user input flushing.
                */
                WiegandCardAndPin(WiegandReaderImpl *reader,
                        std::chrono::milliseconds delay,
                        std::chrono::milliseconds pin_timeout,
                        char pin_end_key);

                virtual void timeout() override;

            private:

                /**
                * Build and dispatch the auth request since we gathered all user input
                * (either due to timeout, or to pin_end_key being pressed).
                *
                * It does nothing if there is no inputs.
                */
                void end_of_input();

                std::string                 pin_;
                std::chrono::milliseconds   delay_;
                std::chrono::milliseconds   pin_timeout_;
                char                        pin_end_key_;

                using TimePoint = std::chrono::system_clock::time_point;
                TimePoint                   last_update_;

                bool reading_card_;
            };
        }
    }
}
