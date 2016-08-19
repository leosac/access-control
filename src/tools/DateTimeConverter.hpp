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

#include "boost/date_time/posix_time/posix_time.hpp"
#include <chrono>

template <typename Duration>
boost::posix_time::ptime time_point_ptime(
    const std::chrono::time_point<std::chrono::system_clock, Duration> &from)
{
    typedef std::chrono::nanoseconds duration_t;
    typedef duration_t::rep rep_t;
    rep_t d =
        std::chrono::duration_cast<duration_t>(from.time_since_epoch()).count();
    rep_t sec  = d / 1000000000;
    rep_t nsec = d % 1000000000;
    return boost::posix_time::from_time_t(0) +
           boost::posix_time::seconds(static_cast<long>(sec)) +
#ifdef BOOST_DATE_TIME_HAS_NANOSECONDS
           boost::posix_time::nanoseconds(nsec);
#else
           boost::posix_time::microseconds((nsec + 500) / 1000);
#endif
}
