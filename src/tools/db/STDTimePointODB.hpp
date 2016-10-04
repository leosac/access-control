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

#include "tools/MyTime.hpp"
#include <cassert>
#include <chrono>
#include <iomanip>
#include <odb/pgsql/traits.hxx>

namespace odb
{

// For PGSql

namespace pgsql
{
/**
 * This is inspired by the boost ODB profile.
 * This contains a few copy-paste from the ODB profile.
 *
 * TIMESTAMP values are stored as micro-seconds
 * since the PostgreSQL epoch 2000-01-01.
 */
template <>
class value_traits<std::chrono::system_clock::time_point, id_timestamp>
{
  public:
    using TimePoint = std::chrono::system_clock::time_point;
    typedef TimePoint value_type;
    typedef value_type query_type;
    typedef long long image_type;

    // Could somewhat map to std::time_point::min()
    static const long long neg_inf = -0x7fffffffffffffffLL - 1;

    // Could somewhat map to std::time_point::max()
    static const long long pos_inf = 0x7fffffffffffffffLL;

    static void set_value(TimePoint &v, image_type i, bool is_null)
    {
        // We cannot (AFAIK) support null date time.
        assert(!is_null);

        i = details::endian_traits::ntoh(i);
        if (i == neg_inf)
            v = TimePoint::min();
        else if (i == pos_inf)
            v = TimePoint::max();
        else
        {
            // todo we probably need to check for overflow.
            std::chrono::microseconds us(i);
            TimePoint epoch = pg_epoch();
            TimePoint ts    = epoch + us;

            v = ts;
        }
    }

    static void set_image(image_type &i, bool &is_null, const TimePoint &v)
    {
        is_null = false;
        if (v == TimePoint::max())
            i = pos_inf;
        else if (v == TimePoint::min())
            i = neg_inf;
        else
        {
            // todo we probably need to check for overflow.
            TimePoint epoch = pg_epoch();
            std::chrono::microseconds us =
                std::chrono::duration_cast<std::chrono::microseconds>(v - epoch);
            i = us.count();
        }
        i = details::endian_traits::hton(i);
    }

  private:
    /**
     * Return a timepoint that represents the PGSQL TIMESTAMP epoch. (2000/01/01 0:0)
     */
    static TimePoint pg_epoch()
    {
        TimePoint pg_epoch;
        std::tm tm;
        bzero(&tm, sizeof(tm));
        std::string epoch = "2000-01-01T00:00:00Z";
        std::istringstream iss(epoch);

        iss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
        assert(iss.good());
        tm.tm_isdst = 0;

        return std::chrono::system_clock::from_time_t(Leosac::my_timegm(&tm));
    }
};
}
}
