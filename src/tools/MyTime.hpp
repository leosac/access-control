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

#pragma once

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <string.h>
#include <string>

namespace Leosac
{

/**
 * This is a portable implementation of the timegm() function.
 *
 * It converts a `tm` tp a `time_t` in UTC.
 */
time_t my_timegm(struct tm *tm);

bool my_gettime(std::tm *out, const std::string &date_str, const char *fmt);
bool my_puttime(std::string &out, const std::tm *tt, const char *fmt);

/**
 * Convert a timepoint to an RFC2822 (SMTP) date.
 *
 * The time represent the local time.
 */
std::string to_local_rfc2822(const std::chrono::system_clock::time_point &tp);
}
