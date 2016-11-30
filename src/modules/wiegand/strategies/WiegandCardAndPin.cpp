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

#include "WiegandCardAndPin.hpp"
#include "modules/wiegand/WiegandReaderImpl.hpp"
#include <tools/log.hpp>

using namespace Leosac::Module::Wiegand;
using namespace Leosac::Module::Wiegand::Strategy;

WiegandCardAndPin::WiegandCardAndPin(WiegandReaderImpl *reader,
                                     CardReadingUPtr read_card,
                                     PinReadingUPtr read_pin,
                                     std::chrono::milliseconds delay)
    : WiegandStrategy(reader)
    , read_card_strategy_(std::move(read_card))
    , read_pin_strategy_(std::move(read_pin))
    , delay_(delay)
    , reading_card_(true)
    , ready_(false)
{
    time_card_read_ = std::chrono::system_clock::now();
}

void WiegandCardAndPin::timeout()
{
    using namespace std::chrono;
    auto elapsed_ms =
        duration_cast<milliseconds>(system_clock::now() - time_card_read_);

    if (reading_card_)
    {
        read_card_strategy_->timeout();
        if (read_card_strategy_->completed())
        {
            DEBUG("Switch to PIN. Current card id = "
                  << read_card_strategy_->get_card_id());
            reading_card_   = false;
            time_card_read_ = std::chrono::system_clock::now();
            reader_->read_reset();
        }
    }
    else
    {
        if (elapsed_ms > delay_)
        {
            DEBUG("Too slow to enter pin code. Aborting.");
            reset();
            return;
        }
        // if we received anything, update the last activity time to give
        // more time to user to eventually finish to type its code.
        if (reader_->counter())
            time_card_read_ = std::chrono::system_clock::now();
        read_pin_strategy_->timeout();
        if (read_pin_strategy_->completed())
            ready_ = true;
    }
}

bool WiegandCardAndPin::completed() const
{
    return ready_;
}

void WiegandCardAndPin::signal(zmqpp::socket &sock)
{
    DEBUG("Card = " << read_card_strategy_->get_card_id());
    DEBUG("Pin = " << read_pin_strategy_->get_pin());

    zmqpp::message msg;
    msg << ("S_" + reader_->name()) << Auth::SourceType::WIEGAND_CARD_PIN
        << read_card_strategy_->get_card_id() << read_card_strategy_->get_nb_bits()
        << read_pin_strategy_->get_pin();
    sock.send(msg);
    reset();
}

void WiegandCardAndPin::set_reader(WiegandReaderImpl *new_ptr)
{
    WiegandStrategy::set_reader(new_ptr);
    read_card_strategy_->set_reader(new_ptr);
    read_pin_strategy_->set_reader(new_ptr);
}

void WiegandCardAndPin::reset()
{
    ready_        = false;
    reading_card_ = true;
    reader_->read_reset();

    read_card_strategy_->reset();
    read_pin_strategy_->reset();
}
