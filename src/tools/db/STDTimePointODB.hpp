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

#include "tools/MyTime.hpp"
#include <cassert>
#include <chrono>
#include <date/date.h>
#include <iomanip>
#include <odb/pgsql/traits.hxx>
#include <odb/sqlite/traits.hxx>

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

        std::chrono::system_clock::time_point tp;
        auto x = date::parse("%FT%T%Z", tp);
        std::istringstream iss(epoch);
        iss >> x;
        return tp;
    }
};
}


// For SQLite

namespace sqlite
{
/**
 * Store as string.
 */
template <>
class value_traits<std::chrono::system_clock::time_point, id_text>
{
  public:
    using TimePoint = std::chrono::system_clock::time_point;
    typedef TimePoint value_type;
    typedef value_type query_type;
    typedef details::buffer image_type;

    static constexpr const char *const TIME_POINT_MIN = "TIME_POINT_MIN";
    static constexpr const char *const TIME_POINT_MAX = "TIME_POINT_MAX";

    static void set_value(TimePoint &v, const details::buffer &b, std::size_t n,
                          bool is_null)
    {
        assert(!is_null);
        std::string str_rep(b.data(), n);

        if (str_rep == TIME_POINT_MAX)
            v = TimePoint::max();
        else if (str_rep == TIME_POINT_MIN)
            v = TimePoint::min();
        else
        {
            auto x = date::parse("%FT%T%z", v);
            std::istringstream iss(str_rep);
            iss >> x;
            assert(iss.good());
        }
    }

    static void set_image(details::buffer &b, std::size_t &n, bool &is_null,
                          const TimePoint &v)
    {
        is_null = false;
        std::string str_rep;

        if (v == TimePoint::max())
            str_rep = TIME_POINT_MAX;
        else if (v == TimePoint::min())
            str_rep = TIME_POINT_MIN;
        else
            str_rep = date::format("%FT%T%z", v);

        n = str_rep.size();
        if (str_rep.size() > b.capacity())
            b.capacity(str_rep.length());
        std::memcpy(b.data(), str_rep.data(), str_rep.length());
    }
};
}
}
