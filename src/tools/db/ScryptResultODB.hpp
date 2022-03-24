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

#include "tools/scrypt/Scrypt.hpp"
#include <odb/pgsql/traits.hxx>
#include <odb/sqlite/traits.hxx>

/**
 * Provide ODB magic to be able to store a ScryptResult (password salt + hash)
 * in the database.
 *
 * The object is stored as text.
 */

namespace odb
{

// For PGSql

namespace pgsql
{
template <>
class value_traits<ScryptResult, id_string>
{
  public:
    typedef ScryptResult value_type;
    typedef value_type query_type;
    typedef details::buffer image_type;

    static void set_value(ScryptResult &v, const details::buffer &b, std::size_t n,
                          bool is_null)
    {
        if (!is_null)
        {
            std::string str_rep(b.data(), n);
            StringScryptResultSerializer s;
            v = s.UnSerialize(str_rep);
        }
        else
            v = ScryptResult();
    }

    static void set_image(details::buffer &b, std::size_t &n, bool &is_null,
                          const ScryptResult &v)
    {
        is_null = false;
        StringScryptResultSerializer s;
        std::string str_rep = s.Serialize(v);

        n = str_rep.size();
        if (str_rep.size() > b.capacity())
            b.capacity(str_rep.length());
        std::memcpy(b.data(), str_rep.data(), str_rep.length());
    }
};
}

// For SQLite

namespace sqlite
{
template <>
class value_traits<ScryptResult, id_text>
{
  public:
    typedef ScryptResult value_type;
    typedef value_type query_type;
    typedef details::buffer image_type;

    static void set_value(ScryptResult &v, const details::buffer &b, std::size_t n,
                          bool is_null)
    {
        if (!is_null)
        {
            std::string str_rep(b.data(), n);
            StringScryptResultSerializer s;
            v = s.UnSerialize(str_rep);
        }
        else
            v = ScryptResult();
    }

    static void set_image(details::buffer &b, std::size_t &n, bool &is_null,
                          const ScryptResult &v)
    {
        is_null = false;
        StringScryptResultSerializer s;
        std::string str_rep = s.Serialize(v);

        n = str_rep.size();
        if (str_rep.size() > b.capacity())
            b.capacity(str_rep.length());
        std::memcpy(b.data(), str_rep.data(), str_rep.length());
    }
};
}
}
