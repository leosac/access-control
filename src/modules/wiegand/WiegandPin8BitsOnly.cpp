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
#include "WiegandPin8BitsOnly.hpp"

using namespace Leosac::Module::Wiegand;


WiegandPin8BitsOnly::WiegandPin8BitsOnly(WiegandReaderImpl *reader,
        std::chrono::milliseconds pin_timeout,
        char pin_end_key) :
        WiegandStrategy(reader),
        pin_timeout_(pin_timeout),
        pin_end_key_(pin_end_key),
        ready_(false)
{
    last_update_ = std::chrono::system_clock::now();
}

void WiegandPin8BitsOnly::timeout()
{
    using namespace std::chrono;
    auto elapsed_ms = duration_cast<milliseconds>(system_clock::now() - last_update_);

    if (!reader_->counter())
    {
        if (elapsed_ms > pin_timeout_)
            end_of_input();
        return;
    }

    if (reader_->counter() != 8)
    {
        WARN("Expected number of bits invalid. (" << reader_->counter() << " but we expected 8)");
        reader_->read_reset();
        inputs_ = "";
        return;
    }

    last_update_ = system_clock::now();
    // buffer[0] = 1010 0101
    //          reverse wxyz
    // w is first byte, x is second, ...
    // so this example has value of 5
    unsigned int n = 0;
    for (int i = 0; i < 4; ++i)
    {
        unsigned int v = ((reader_->buffer()[0] >> (7 - i)) & 0x01);
        n |= !v << (3 - i);
    }
    // this deduced from manual testing. Value for '#' and '*'.
    // 10 -> *
    // 11 -> #
    char c;
    if (n == 10)
        c = '*';
    else if (n == 11)
        c = '#';
    else
        c = std::to_string(n).at(0);
    if (c == pin_end_key_)
    {
        end_of_input();
    }
    else
        inputs_ += c;
    reader_->read_reset();
}

void WiegandPin8BitsOnly::end_of_input()
{
    if (inputs_.length())
        ready_ = true;
    else
        ready_ = false;
}

bool WiegandPin8BitsOnly::completed() const
{
    return ready_;
}

void WiegandPin8BitsOnly::signal()
{
    assert(ready_);
    assert(inputs_.length());
    DEBUG("Sending PIN Code: " << inputs_);
    zmqpp::message msg;
    msg << ("S_" + reader_->name()) << Leosac::Auth::SourceType::WIEGAND_PIN << inputs_;
    reader_->bus_push_.send(msg);
    reader_->read_reset();
    ready_ = false;
    inputs_ = "";
}

