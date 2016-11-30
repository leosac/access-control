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

#include "WiegandPinNBitsOnly.hpp"
#include "modules/wiegand/WiegandReaderImpl.hpp"
#include <tools/log.hpp>

using namespace Leosac::Module::Wiegand;
using namespace Leosac::Module::Wiegand::Strategy;

template <unsigned int NbBits>
WiegandPinNBitsOnly<NbBits>::WiegandPinNBitsOnly(
    WiegandReaderImpl *reader, std::chrono::milliseconds pin_timeout,
    char pin_end_key)
    : PinReading(reader)
    , pin_timeout_(pin_timeout)
    , pin_end_key_(pin_end_key)
    , ready_(false)
{
    last_update_ = std::chrono::system_clock::now();
}

template <unsigned int NbBits>
char WiegandPinNBitsOnly<NbBits>::extract_from_raw(uint8_t input) const
{
    // If we receive 4 bits per key, the
    // data looks like this:
    // buffer[0] = 0101 ...
    //             ^^^^ key pressed.
    // For 8 bits mode, it looks like this:
    // buffer[0] = 1010 0101
    //        ~bits^^^^ ^^^^ key value
    // If this example, the key pressed is '5'.
    DEBUG("Buffer value = " << (unsigned int)input);
    unsigned int n = 0;
    for (int i = 0; i < 4; ++i)
    {
        bool v = ((input >> (7 - i)) & 0x01);
        if (NbBits == 8)
            v = !v;
        n |= v << (3 - i);
    }
    // this deduced from manual testing. Value for '#' and '*'.
    // 10 -> *
    // 11 -> #
    if (n == 10)
        return '*';
    else if (n == 11)
        return '#';
    else
        return std::to_string(n).at(0);
}

template <unsigned int NbBits>
void WiegandPinNBitsOnly<NbBits>::timeout()
{
    static_assert(NbBits == 4 || NbBits == 8,
                  "Must either be 4 or 8 bits per key pressed");
    using namespace std::chrono;
    auto elapsed_ms =
        duration_cast<milliseconds>(system_clock::now() - last_update_);

    if (!reader_->counter())
    {
        if (elapsed_ms > pin_timeout_)
            end_of_input();
        return;
    }

    if (reader_->counter() != NbBits)
    {
        WARN("Expected number of bits invalid. ("
             << reader_->counter() << " but we expected " << NbBits << ")");
        reset();
        return;
    }

    last_update_ = system_clock::now();
    char c       = extract_from_raw(reader_->buffer()[0]);
    if (c == pin_end_key_)
    {
        end_of_input();
    }
    else
        inputs_ += c;
    reader_->read_reset();
}

template <unsigned int NbBits>
void WiegandPinNBitsOnly<NbBits>::end_of_input()
{
    if (inputs_.length())
        ready_ = true;
    else
        ready_ = false;
}

template <unsigned int NbBits>
bool WiegandPinNBitsOnly<NbBits>::completed() const
{
    return ready_;
}

template <unsigned int NbBits>
void WiegandPinNBitsOnly<NbBits>::signal(zmqpp::socket &sock)
{
    assert(ready_);
    assert(inputs_.length());
    DEBUG("Sending PIN Code: " << inputs_);
    zmqpp::message msg;
    msg << ("S_" + reader_->name()) << Leosac::Auth::SourceType::WIEGAND_PIN
        << inputs_;
    sock.send(msg);
    reset();
}

template <unsigned int NbBits>
const std::string &WiegandPinNBitsOnly<NbBits>::get_pin() const
{
    return inputs_;
}

template <unsigned int NbBits>
void WiegandPinNBitsOnly<NbBits>::reset()
{
    reader_->read_reset();
    ready_       = false;
    inputs_      = "";
    last_update_ = std::chrono::system_clock::now();
}

namespace Leosac
{
namespace Module
{
namespace Wiegand
{
namespace Strategy
{
template class WiegandPinNBitsOnly<4>;
template class WiegandPinNBitsOnly<8>;
}
}
}
}
