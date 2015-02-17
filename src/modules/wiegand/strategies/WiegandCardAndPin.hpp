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
                            CardReadingUPtr read_card,
                            PinReadingUPtr read_pin,
                            std::chrono::milliseconds delay);

                    virtual void timeout() override;

                    virtual bool completed() const override;

                    virtual void signal() override;

                    virtual void set_reader(WiegandReaderImpl *new_ptr) override;

                private:

                    /**
                    * Reset self.
                    * We create new strategy instance, reset the boolean flag to defaults and
                    * reset the underlying reader buffer.
                    */
                    void reset() override;

                    CardReadingUPtr read_card_strategy_;
                    PinReadingUPtr read_pin_strategy_;

                    std::chrono::milliseconds delay_;
                    using TimePoint = std::chrono::system_clock::time_point;
                    TimePoint time_card_read_;

                    bool reading_card_;
                    bool ready_;
                };
            }
        }
    }
}
