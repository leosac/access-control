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

#pragma once

#include <cassert>
#include <chrono>
#include <string>

namespace Leosac
{

/**
 * Declaration of the conversion function.
 *
 * This template function is not provided. Instead,
 * each conversion should be implemented as a specialization of this function
 * template.
 */
template <typename To, typename From>
To Conversion(const From &in);

/**
 * Converts a time_point to an ISO date string.
 */
template <>
std::string Conversion<std::string, std::chrono::system_clock::time_point>(
    const std::chrono::system_clock::time_point &tp);
}
