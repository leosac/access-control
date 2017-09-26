/*
    Copyright (C) 2014-2017 Leosac

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
#include <odb/sqlite/traits.hxx>

namespace odb
{
/**
 * Code to map std::chrono::milliseconds to an integer in database.
 */

// For PGSql

namespace pgsql
{
template <>
class value_traits<std::chrono::milliseconds, id_bigint>
{
  public:
    typedef std::chrono::milliseconds value_type;
    typedef value_type query_type;
    typedef long long image_type;

    static void set_value(std::chrono::milliseconds &v, image_type i, bool is_null)
    {
        if (is_null)
            v = std::chrono::milliseconds(0);
        else
        {
            v = std::chrono::milliseconds(i);
        }
    }

    static void set_image(image_type &i, bool &is_null,
                          const std::chrono::milliseconds &v)
    {
        is_null = false;
        i       = v.count();
    }
};
}


// For SQLite

namespace sqlite
{
template <>
class value_traits<std::chrono::milliseconds, id_integer>
{
  public:
    typedef std::chrono::milliseconds value_type;
    typedef value_type query_type;
    typedef long long image_type;

    static void set_value(std::chrono::milliseconds &v, image_type i, bool is_null)
    {
        if (is_null)
            v = std::chrono::milliseconds(0);
        else
        {
            v = std::chrono::milliseconds(i);
        }
    }

    static void set_image(image_type &i, bool &is_null,
                          const std::chrono::milliseconds &v)
    {
        is_null = false;
        i       = v.count();
    }
};
}
}
