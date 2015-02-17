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

#include <tools/log.hpp>
#include "WiegandCardAndPin.hpp"

using namespace Leosac::Module::Wiegand;


WiegandCardAndPin::WiegandCardAndPin(WiegandReaderImpl *reader,
        std::chrono::milliseconds delay,
        std::chrono::milliseconds pin_timeout,
        char pin_end_key) :
        WiegandStrategy(reader),
        delay_(delay),
        pin_timeout_(pin_timeout),
        pin_end_key_(pin_end_key),
        reading_card_(true)
{
    last_update_ = std::chrono::system_clock::now();
}

void WiegandCardAndPin::timeout()
{
    if (reading_card_)
    {
        if (!reader_->counter())
            return;

        DEBUG("timeout, buffer size = " << reader_->counter());
        std::size_t size = ((reader_->counter() - 1) / 8) + 1;

        std::stringstream card_hex;

        for (std::size_t i = 0; i < size; ++i)
        {
            card_hex << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(reader_->buffer()[i]);
            if (i + 1 < size)
                card_hex << ":";
        }

        DEBUG("Card = " << card_hex.str());
        reader_->read_reset();
        reading_card_ = false;
    }
    else
    {

    }
}
