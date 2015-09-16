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

#include <boost/algorithm/string.hpp>
#include <sstream>
#include <tools/log.hpp>
#include "tools/IVisitor.hpp"
#include "WiegandCard.hpp"

using namespace Leosac::Auth;

WiegandCard::WiegandCard(const std::string &cardid, int bits) :
        card_id_(cardid),
        nb_bits_(bits)
{

}

void WiegandCard::accept(Leosac::Tools::IVisitor *visitor)
{
    visitor->visit(this);
}

const std::string &WiegandCard::card_id() const
{
    return card_id_;
}

int WiegandCard::nb_bits() const
{
    return nb_bits_;
}

std::string WiegandCard::to_string() const
{
    std::stringstream ss;

    ss << "Text representation of auth source:" << std::endl << "\t\t";
    ss << "Source Name: " << source_name_ << std::endl << "\t\t";
    ss << "Source Type: " << "WiegandCard" << std::endl << "\t\t";
    ss << "Number of bits: " << nb_bits_ << std::endl << "\t\t";
    ss << "Card id: " << card_id();
    return ss.str();
}

uint64_t WiegandCard::to_int() const
{
    switch (nb_bits_)
    {
        case 26:
            return to_wiegand_26();
        default:
        {
            auto card_num_hex = boost::replace_all_copy(card_id_, ":", "");
            return std::stoul(card_num_hex, nullptr, 16);
        }
    }
}

uint64_t WiegandCard::to_wiegand_26() const
{
    assert(nb_bits_ == 26);
    assert(card_id_.size() == 2*4 + 3);

    auto card_num_hex = boost::replace_all_copy(card_id_, ":", "");
    uint64_t tmp = std::stoul(card_num_hex, nullptr, 16);

    // we have 32 bits of data (8 hex character)

    // we want to drop the last bit from wiegand 26 frame.
    // so drop 7 bits (6 useless (32-26) + last one)
    tmp = tmp >> 7;
    // keep 16 bits
    tmp &= 0xFFFF;
    return tmp;
}
