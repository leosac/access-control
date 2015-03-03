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
#include "Autodetect.hpp"
#include "modules/wiegand/WiegandReaderImpl.hpp"

using namespace Leosac::Module::Wiegand;
using namespace Leosac::Module::Wiegand::Strategy;

Autodetect::Autodetect(WiegandReaderImpl *reader,
        std::chrono::milliseconds delay) :
        WiegandStrategy(reader),
        delay_(delay),
        reading_pin_(false),
        reading_card_(true),
        ready_(false)
{
    time_card_read_ = std::chrono::system_clock::now();
    last_pin_read_ =  std::chrono::system_clock::now();
    read_card_strategy_ = std::unique_ptr<CardReading>(new SimpleWiegandStrategy(reader));
}

void Autodetect::timeout()
{
    using namespace std::chrono;
    if (reader_->counter() == 4)
    {
        reading_pin_ = true;
        if (!read_pin_strategy_)
            read_pin_strategy_ = std::unique_ptr<PinReading>(new WiegandPin4BitsOnly(reader_,
                    std::chrono::milliseconds(2000), '#'));
        read_pin_strategy_->set_reader(reader_);
        DEBUG("This is likely a PIN card");
        read_pin_strategy_->timeout();
        last_pin_read_ = system_clock::now();
        if (read_pin_strategy_->completed())
        {
            DEBUG("Read PIN = " << read_pin_strategy_->get_pin());
            ready_ = true;
            reading_pin_ = false;
            reader_->read_reset();
        }
    }
    else if (reader_->counter())
    {
        //  if (elapsed_ms > std::chrono::milliseconds(2000))
        //{
        //  DEBUG("Internal timeout... or something.");
        //}
        read_card_strategy_->timeout();
        DEBUG("DOING SOME CARD READING");
        time_card_read_ = system_clock::now();
        if (read_card_strategy_->completed())
        {
            DEBUG("Read CARD = " << read_card_strategy_->get_card_id());
            reader_->read_reset();
        }
    }

    auto elapsed_ms = duration_cast<milliseconds>(system_clock::now() - time_card_read_);
    auto elapsed_ms_pin = duration_cast<milliseconds>(system_clock::now() - last_pin_read_);


    if (reading_pin_)
    {
        // make sure it timeout sometime.
        if (elapsed_ms_pin > std::chrono::milliseconds(2000))
        {
            read_pin_strategy_->timeout();
            DEBUG("PIN READING TIMEOUT");
            if (read_pin_strategy_->completed())
            {
                DEBUG("Read PIN = " << read_pin_strategy_->get_pin());
                ready_ = true;
                reading_pin_ = false;
                reader_->read_reset();
            }
        }
    }

    // 2 sec of total inactivitry and we valid card.
    if (elapsed_ms > std::chrono::milliseconds(3000)
            && elapsed_ms_pin > std::chrono::milliseconds(3000))
    {
        if (read_card_strategy_->completed() && read_card_strategy_->get_nb_bits())
        {
            ready_ = true;
            DEBUG("BLABLA ALERT DOWN");
        }
    }

}

bool Autodetect::completed() const
{
    return ready_;
}

void Autodetect::signal(zmqpp::socket &sock)
{
    bool with_card  = read_card_strategy_->get_nb_bits();
    bool with_pin   = read_pin_strategy_ && read_pin_strategy_->get_pin().length();

    if (with_card)
        DEBUG("Card = " << read_card_strategy_->get_card_id());
    else
        DEBUG("NO CARD");

    if (with_pin)
        DEBUG("Pin = " << read_pin_strategy_->get_pin());
    else
        DEBUG("NO PIN");

    zmqpp::message msg;
    msg << ("S_" + reader_->name());
    if (with_card && with_pin)
        msg << Auth::SourceType::WIEGAND_CARD_PIN;
    else if (with_card)
        msg << Auth::SourceType::SIMPLE_WIEGAND;
    else if (with_pin)
        msg << Auth::SourceType::WIEGAND_PIN;

    if (with_card)
        msg << read_card_strategy_->get_card_id() << read_card_strategy_->get_nb_bits();
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
    ready_ = false;
    reading_card_ = true;
    reader_->read_reset();

    read_card_strategy_->reset();
    if (read_pin_strategy_)
        read_pin_strategy_->reset();

}
