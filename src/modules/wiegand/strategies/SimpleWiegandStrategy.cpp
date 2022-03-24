/*
    Copyright (C) 2014-2022 Leosac

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

#include "SimpleWiegandStrategy.hpp"
#include "modules/wiegand/WiegandReaderImpl.hpp"
#include <iomanip>
#include <tools/log.hpp>

using namespace Leosac::Module::Wiegand;
using namespace Leosac::Module::Wiegand::Strategy;


SimpleWiegandStrategy::SimpleWiegandStrategy(WiegandReaderImpl *reader)
    : CardReading(reader)
    , ready_(false)
    , nb_bits_(0)
{
}

void SimpleWiegandStrategy::timeout()
{
    if (!reader_->counter())
        return;

    DEBUG("timeout, buffer size = " << reader_->counter());
    std::size_t size = ((reader_->counter() - 1) / 8) + 1;

    std::stringstream card_hex;

    for (std::size_t i = 0; i < size; ++i)
    {
        card_hex << std::hex << std::setfill('0') << std::setw(2)
                 << static_cast<int>(reader_->buffer()[i]);
        if (i + 1 < size)
            card_hex << ":";
    }

    ready_   = true;
    nb_bits_ = reader_->counter();
    card_id_ = card_hex.str();
}

bool SimpleWiegandStrategy::completed() const
{
    return ready_;
}

void SimpleWiegandStrategy::signal(zmqpp::socket &sock)
{
    assert(ready_);
    assert(card_id_.length());

    zmqpp::message msg;
    msg << ("S_" + reader_->name()) << Leosac::Auth::SourceType::SIMPLE_WIEGAND
        << card_id_ << nb_bits_;
    sock.send(msg);
}

const std::string &SimpleWiegandStrategy::get_card_id() const
{
    return card_id_;
}

int SimpleWiegandStrategy::get_nb_bits() const
{
    return nb_bits_;
}

void SimpleWiegandStrategy::reset()
{
    ready_   = false;
    card_id_ = "";
    nb_bits_ = 0;
    reader_->read_reset();
}
