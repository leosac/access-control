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
#include "SimpleWiegandStrategy.hpp"

using namespace Leosac::Module::Wiegand;


SimpleWiegandStrategy::SimpleWiegandStrategy(WiegandReaderImpl *reader) :
        WiegandStrategy(reader)
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
        card_hex << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(reader_->buffer()[i]);
        if (i + 1 < size)
            card_hex << ":";
    }

    zmqpp::message msg;
    msg << ("S_" + reader_->name()) << Leosac::Auth::SourceType::SIMPLE_WIEGAND << card_hex.str() << reader_->counter();
    reader_->bus_push_.send(msg);

    reader_->read_reset();
}
