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
#include "WiegandPin4BitsOnly.hpp"

using namespace Leosac::Module::Wiegand;


WiegandPin4BitsOnly::WiegandPin4BitsOnly(WiegandReaderImpl *reader) :
        WiegandStrategy(reader)
{
    last_update_ = std::chrono::system_clock::now();
}

void WiegandPin4BitsOnly::timeout()
{
    using namespace std::chrono;
    auto elapsed_ms = duration_cast<milliseconds>(system_clock::now() - last_update_);

    if (!reader_->counter_)
    {
        // timeout due to inactivity. check how many time since timeout, if more than 1500ms,
        // full saved number
        if (elapsed_ms.count() > 1500 && inputs_.length())
        {
            // we reset after 1500 ms
            DEBUG("Reset");
            reader_->counter_ = 0;
            DEBUG("Current number streak: " << inputs_);
            inputs_ = "";
        }
        return;
    }

    if (reader_->counter_ != 4)
    {
        WARN("Expected number of bits invalid. (" << reader_->counter_ << " but we expected 4)");
        reader_->counter_ = 0;
        return;
    }

    last_update_ = system_clock::now();
    // store the number. sent in 4bits.
    // sent in little endian, but main wiegand store in differently (first received is big)

    // buffer[0] = 1010 0000
    //             wxyz
    // w is first byte, x is second, ...
    // so this example has value of 5

    unsigned int n = 0;
    DEBUG("Counter = " << reader_->counter_);
    for (int i = 0; i < 4; ++i)
    {
        unsigned int v = ((reader_->buffer_[0] >> (7 - i)) & 0x01);
        n |= v << (3 - i);
    }

    DEBUG("Registered key {" << n << "}");
    // this deduced from manual testing
    // 10 -> *
    // 11 -> #
    if (n == 10)
        inputs_ += '*';
    else if (n == 1)
        inputs_ += '#';
    else
        inputs_ += std::to_string(n);
    reader_->counter_ = 0;
}
