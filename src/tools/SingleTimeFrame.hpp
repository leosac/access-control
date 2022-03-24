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

#pragma once

#include <chrono>

namespace Leosac
{
namespace Tools
{
/**
 * This struct abstracts what we call a single time frame.
 *
 * A single time frame represents a time slice in a single given day.
 * It's comprised of:
 *    + A day of the week
 *    + A start hour/minute
 *    + An end hour/minute
 */
#pragma db value
struct SingleTimeFrame
{

    /**
     * Construct a SingleTimeFrame.
     *
     * @param d The day of the week, starts at 0 for monday.
     * @param sh The starting hour.
     * @param sm The starting minute.
     * @param eh The ending hour.
     * @param em The ending minute.
     */
    SingleTimeFrame(int d, int sh, int sm, int eh, int em);

    SingleTimeFrame();

    SingleTimeFrame(const SingleTimeFrame &) = default;

    bool operator==(const SingleTimeFrame &o) const;
    bool operator<(const SingleTimeFrame &o) const;

    int day;
    int start_hour;
    int start_min;
    int end_hour;
    int end_min;

    /**
    * Is the given timepoint in the time frame ?
    */
    bool is_in_timeframe(const std::chrono::system_clock::time_point &tp) const;
};
}
}
