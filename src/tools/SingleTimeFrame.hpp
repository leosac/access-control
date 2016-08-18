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

#include <chrono>

#pragma once

namespace Leosac
{
namespace Tools
{
/**
* This struct abstract a single time frame.
*
* This single time frame is composed of a day of the week, a start
* and a end hour.
*/
struct SingleTimeFrame
{

    SingleTimeFrame(int d, int sh, int sm, int eh, int em)
        : day(d)
        , start_hour(sh)
        , start_min(sm)
        , end_hour(eh)
        , end_min(em)
    {
    }

    SingleTimeFrame()
        : day(0)
        , start_hour(0)
        , start_min(0)
        , end_hour(0)
        , end_min(0)
    {
    }

    SingleTimeFrame(const SingleTimeFrame &) = default;

    int day;
    int start_hour;
    int start_min;
    int end_hour;
    int end_min;

    /**
    * Is the given timepoint in the time frame ?
    */
    bool is_in_timeframe(const std::chrono::system_clock::time_point &tp) const
    {
        std::time_t time_temp   = std::chrono::system_clock::to_time_t(tp);
        std::tm const *time_out = std::localtime(&time_temp);

        if (this->day != time_out->tm_wday)
            return false;
        if (!(this->start_hour < time_out->tm_hour ||
              (this->start_hour == time_out->tm_hour &&
               this->start_min <= time_out->tm_min)))
            return false;
        if (!(this->end_hour > time_out->tm_hour ||
              (this->end_hour == time_out->tm_hour &&
               this->end_min >= time_out->tm_min)))
            return false;
        return true;
    }
};
}
}
