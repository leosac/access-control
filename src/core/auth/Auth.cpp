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

#include "Auth.hpp"

zmqpp::message &operator<<(zmqpp::message &msg, const Leosac::Auth::SourceType &t)
{
    static_assert(std::is_same<std::underlying_type<Leosac::Auth::SourceType>::type, uint8_t>::value,
            "Bad underlying type for enum");
    msg << static_cast<uint8_t>(t);
    return msg;
}

zmqpp::message &operator>>(zmqpp::message &msg, Leosac::Auth::SourceType &t)
{
    static_assert(std::is_same<std::underlying_type<Leosac::Auth::SourceType>::type, uint8_t>::value,
            "Bad underlying type for enum");
    uint8_t tmp;
    msg >> tmp;
    t = static_cast<Leosac::Auth::SourceType>(tmp);
    return msg;
}

zmqpp::message &operator<<(zmqpp::message &msg, const Leosac::Auth::AccessStatus &st)
{
    static_assert(std::is_same<std::underlying_type<Leosac::Auth::AccessStatus>::type, uint8_t>::value,
            "Bad underlying type for enum");
    msg << static_cast<uint8_t>(st);
    return msg;
}

zmqpp::message &operator>>(zmqpp::message &msg, Leosac::Auth::AccessStatus &st)
{
    static_assert(std::is_same<std::underlying_type<Leosac::Auth::AccessStatus>::type, uint8_t>::value,
            "Bad underlying type for enum");
    uint8_t tmp;
    msg >> tmp;
    st = static_cast<Leosac::Auth::AccessStatus>(tmp);
    return msg;
}
