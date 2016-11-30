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

#include "WiegandPinBuffered.hpp"
#include "modules/wiegand/WiegandReaderImpl.hpp"
#include <tools/log.hpp>

using namespace Leosac::Module::Wiegand;
using namespace Leosac::Module::Wiegand::Strategy;

WiegandPinBuffered::WiegandPinBuffered(WiegandReaderImpl *reader)
    : PinReading(reader)
    , ready_(false)
{
}

void WiegandPinBuffered::timeout()
{
    if (reader_->counter() == 0)
    {
        // fail silently since its a normal timeout.
        return;
    }
    if (reader_->counter() != 26)
    {
        WARN("Expected number of bits invalid. (" << reader_->counter()
                                                  << " but we expected 26)");
        reset();
        return;
    }

    // bits 10 to 25 are relevant.
    // in MSB
    unsigned int n = 0;
    for (int i = 9; i < 26; ++i)
    {
        unsigned int v = ((reader_->buffer()[i / 8] >> (7 - i % 8)) & 0x01);
        n |= v << (15 - (i - 9));
    }
    if (n == 65535)
    {
        // per HID documentation.
        WARN("Invalid Pin Code");
        return;
    }
    pin_   = std::to_string(n);
    ready_ = true;
}

bool WiegandPinBuffered::completed() const
{
    return ready_;
}

void WiegandPinBuffered::signal(zmqpp::socket &sock)
{
    assert(ready_);
    assert(pin_.length());

    DEBUG("Sending PIN Code: " << pin_);
    zmqpp::message msg;
    msg << ("S_" + reader_->name()) << Leosac::Auth::SourceType::WIEGAND_PIN << pin_;
    sock.send(msg);
}

const std::string &WiegandPinBuffered::get_pin() const
{
    return pin_;
}

void WiegandPinBuffered::reset()
{
    reader_->read_reset();
    ready_ = false;
    pin_   = "";
}
