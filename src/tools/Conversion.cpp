/*
    Copyright (C) 2014-2016 Islog

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

#include "tools/Conversion.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

namespace Leosac
{


template <>
std::string Conversion<std::string, std::chrono::system_clock::time_point>(
    const std::chrono::system_clock::time_point &tp)
{
    std::stringstream ss;
    std::time_t dt_time_t = std::chrono::system_clock::to_time_t(tp);
    ss << std::put_time(std::gmtime(&dt_time_t), "%FT%T%z");
    return ss.str();
}
}
