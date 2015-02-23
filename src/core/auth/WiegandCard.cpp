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

#include <tools/IVisitor.hpp>
#include <sstream>
#include "WiegandCard.hpp"

using namespace Leosac::Auth;

WiegandCard::WiegandCard(const std::string &card_id, int bits) :
        id_(card_id),
        nb_bits_(bits)
{

}

void WiegandCard::accept(Leosac::Tools::IVisitor *visitor)
{
    visitor->visit(this);
}

const std::string &WiegandCard::id() const
{
    return id_;
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
    ss << "Card id: " << id();
    return ss.str();
}
