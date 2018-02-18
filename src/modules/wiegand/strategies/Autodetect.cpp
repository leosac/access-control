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

#include "Autodetect.hpp"
#include "modules/wiegand/WiegandReaderImpl.hpp"
#include <tools/log.hpp>

using namespace Leosac::Module::Wiegand;
using namespace Leosac::Module::Wiegand::Strategy;

Autodetect::Autodetect(WiegandReaderImpl *reader, std::chrono::milliseconds delay,
                       char pin_key_end, bool nowait)
    : WiegandStrategy(reader)
    , delay_(delay)
    , reading_pin_(false)
    , reading_card_(true)
    , ready_(false)
    , pin_key_end_(pin_key_end)
    , nowait_(nowait)
{
    time_card_read_ = std::chrono::system_clock::now();
    last_pin_read_  = std::chrono::system_clock::now();
    read_card_strategy_ =
        std::unique_ptr<CardReading>(new SimpleWiegandStrategy(reader));
}

void Autodetect::timeout()
{
    using namespace std::chrono;

    // When finishing reading a PIN code we mark as ready.
    // This is because we have a choice between: PIN / Card / Card + PIN, so if PIN
    // is presents,
    // it is always last.

    if (reader_->counter() == 4 || reader_->counter() == 8)
    {
        reading_pin_ = true;
        if (!read_pin_strategy_)
            read_pin_strategy_ = build_strategy(reader_->counter());
        read_pin_strategy_->set_reader(reader_);
        DEBUG("This is likely a PIN code");

        read_pin_strategy_->timeout();
        last_pin_read_ = system_clock::now();
        if (read_pin_strategy_->completed())
        {
            DEBUG("Read PIN = " << read_pin_strategy_->get_pin());
            ready_       = true;
            reading_pin_ = false;
            reader_->read_reset();
        }
    }
    else if (reader_->counter())
    {
        read_card_strategy_->timeout();
        DEBUG("DOING SOME CARD READING");
        time_card_read_ = system_clock::now();
        if (read_card_strategy_->completed())
        {
            DEBUG("Read CARD = " << read_card_strategy_->get_card_id());
            reader_->read_reset();
        }
    }
    else
    {
        check_timeout();
    }
}

void Autodetect::check_timeout()
{
    using namespace std::chrono;

    // Nothing was read, this is an inactivity timeout() tick.
    // We do 2 things here: If we were reading a PIN code, check timeout and finish
    // if timeout
    // Check inactivity timeout for card + pin, to timeout and send a Card only
    // credentials.

    auto elapsed_ms =
        duration_cast<milliseconds>(system_clock::now() - time_card_read_);
    auto elapsed_ms_pin =
        duration_cast<milliseconds>(system_clock::now() - last_pin_read_);

    if (reading_pin_)
    {
        if (elapsed_ms_pin > delay_)
        {
            read_pin_strategy_->timeout();
            DEBUG("PIN READING TIMEOUT");
            if (read_pin_strategy_->completed())
            {
                DEBUG("Read PIN = " << read_pin_strategy_->get_pin());
                ready_       = true;
                reading_pin_ = false;
                reader_->read_reset();
            }
        }
    }
    // 2 sec of total inactivity and we valid card.
    if ((elapsed_ms > delay_ && elapsed_ms_pin > delay_) || nowait_)
    {
        if (read_card_strategy_->completed() && read_card_strategy_->get_nb_bits())
        {
            // mark as ready since no pin were typed after swiping the card
            // for delay_ milliseconds.
            ready_ = true;
        }
    }
}

bool Autodetect::completed() const
{
    return ready_;
}

void Autodetect::signal(zmqpp::socket &sock)
{
    bool with_card = read_card_strategy_->get_nb_bits();
    bool with_pin  = read_pin_strategy_ && read_pin_strategy_->get_pin().length();

    zmqpp::message msg;
    msg << ("S_" + reader_->name());
    if (with_card && with_pin)
        msg << Auth::SourceType::WIEGAND_CARD_PIN;
    else if (with_card)
        msg << Auth::SourceType::SIMPLE_WIEGAND;
    else if (with_pin)
        msg << Auth::SourceType::WIEGAND_PIN;

    if (with_card)
        msg << read_card_strategy_->get_card_id()
            << read_card_strategy_->get_nb_bits();
    if (with_pin)
        msg << read_pin_strategy_->get_pin();

    sock.send(msg);
    reset();
}

void Autodetect::set_reader(WiegandReaderImpl *new_ptr)
{
    WiegandStrategy::set_reader(new_ptr);
    read_card_strategy_->set_reader(new_ptr);
    if (read_pin_strategy_)
        read_pin_strategy_->set_reader(new_ptr);
}

void Autodetect::reset()
{
    ready_        = false;
    reading_card_ = true;
    reader_->read_reset();
    read_card_strategy_->reset();

    // we hard-reset the pin strategy so we can instantiate a new one
    // next time. We can then switch between 4 or 8 bits mode without problem.
    read_pin_strategy_ = nullptr;
}

PinReadingUPtr Autodetect::build_strategy(int bits)
{
    assert(bits == 4 || bits == 8);
    if (bits == 4)
    {
        return std::unique_ptr<PinReading>(
            new WiegandPinNBitsOnly<4>(reader_, delay_, pin_key_end_));
    }
    else if (bits == 8)
    {
        return std::unique_ptr<PinReading>(
            new WiegandPinNBitsOnly<8>(reader_, delay_, pin_key_end_));
    }
    return nullptr;
}
