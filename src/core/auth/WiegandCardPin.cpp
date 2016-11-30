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

#include "WiegandCardPin.hpp"
#include <sstream>

using namespace Leosac::Auth;

WiegandCardPin::WiegandCardPin(const std::string &card_id, int nb_bits,
                               const std::string &pin_code)
    : card_(card_id, nb_bits)
    , pin_(pin_code)
{
}

const WiegandCard &WiegandCardPin::card() const
{
    return card_;
}

const PINCode &WiegandCardPin::pin() const
{
    return pin_;
}

std::string WiegandCardPin::to_string() const
{
    std::stringstream ss;

    ss << "Text representation of auth source:" << std::endl << "\t\t";
    ss << "Source Name: " << source_name_ << std::endl << "\t\t";
    ss << "Source Type: "
       << "WiegandCard + Pin Code" << std::endl
       << "\t\t";
    ss << "Number of bits: " << card_.nb_bits() << std::endl << "\t\t";
    ss << "Card id: " << card_.card_id() << std::endl << "\t\t";
    ss << "Pin Code: " << pin_.pin_code();
    return ss.str();
}
