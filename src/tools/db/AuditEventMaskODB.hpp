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

#include "core/audit/AuditFwd.hpp"
#include <odb/mysql/traits.hxx>
#include <odb/pgsql/traits.hxx>
#include <odb/sqlite/traits.hxx>


/**
 * Provide ODB magic to be able to store an Leosac::Audit::EventType
 * (FlagSet) object.
 */

namespace odb
{

// For MySQL

namespace mysql
{
template <>
class value_traits<Leosac::Audit::EventMask, id_string>
{
  public:
    typedef Leosac::Audit::EventMask value_type;
    typedef value_type query_type;
    typedef details::buffer image_type;

    static void set_value(Leosac::Audit::EventMask &v, const details::buffer &b,
                          std::size_t n, bool is_null)
    {
        if (!is_null)
        {
            std::string bitset_rep(b.data(), n);
            v = Leosac::Audit::EventMask(bitset_rep);
        }
        else
            v.reset();
    }

    static void set_image(details::buffer &b, std::size_t &n, bool &is_null,
                          const Leosac::Audit::EventMask &v)
    {
        is_null                = false;
        std::string bitset_rep = v.get_bitset().to_string();

        n = bitset_rep.size();
        if (bitset_rep.length() > b.capacity())
            b.capacity(bitset_rep.length());
        std::memcpy(b.data(), bitset_rep.data(), bitset_rep.length());
    }
};
}


// For SQLite

namespace sqlite
{
template <>
class value_traits<Leosac::Audit::EventMask, id_text>
{
  public:
    typedef Leosac::Audit::EventMask value_type;
    typedef value_type query_type;
    typedef details::buffer image_type;

    static void set_value(Leosac::Audit::EventMask &v, const details::buffer &b,
                          std::size_t n, bool is_null)
    {
        if (!is_null)
        {
            std::string bitset_rep(b.data(), n);
            v = Leosac::Audit::EventMask(bitset_rep);
        }
        else
            v.reset();
    }

    static void set_image(details::buffer &b, std::size_t &n, bool &is_null,
                          const Leosac::Audit::EventMask &v)
    {
        is_null                = false;
        std::string bitset_rep = v.get_bitset().to_string();

        n = bitset_rep.size();
        if (bitset_rep.length() > b.capacity())
            b.capacity(bitset_rep.length());
        std::memcpy(b.data(), bitset_rep.data(), bitset_rep.length());
    }
};
}

// For PGSql

namespace pgsql
{
template <>
class value_traits<Leosac::Audit::EventMask, id_string>
{
  public:
    typedef Leosac::Audit::EventMask value_type;
    typedef value_type query_type;
    typedef details::buffer image_type;

    static void set_value(Leosac::Audit::EventMask &v, const details::buffer &b,
                          std::size_t n, bool is_null)
    {
        if (!is_null)
        {
            std::string bitset_rep(b.data(), n);
            v = Leosac::Audit::EventMask(bitset_rep);
        }
        else
            v.reset();
    }

    static void set_image(details::buffer &b, std::size_t &n, bool &is_null,
                          const Leosac::Audit::EventMask &v)
    {
        is_null                = false;
        std::string bitset_rep = v.get_bitset().to_string();

        n = bitset_rep.size();
        if (bitset_rep.length() > b.capacity())
            b.capacity(bitset_rep.length());
        std::memcpy(b.data(), bitset_rep.data(), bitset_rep.length());
    }
};
}
}
