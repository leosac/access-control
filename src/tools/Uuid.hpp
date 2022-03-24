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

#include <boost/lexical_cast.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <odb/core.hxx>
#include <string>

namespace Leosac
{
/**
 * Thin wrapper around boost::uuids::uuid.
 */
#pragma db value
class UUID
{
  public:
    UUID() = default;

    UUID(const UUID &) = default;

    explicit UUID(const boost::uuids::uuid &uuid)
        : uuid_(uuid)
    {
    }

    bool operator==(const UUID &other) const
    {
        return uuid_ == other.uuid_;
    }

    bool operator!=(const UUID &other) const
    {
        return uuid_ != other.uuid_;
    }

    bool is_nil() const
    {
        return uuid_.is_nil();
    }

    std::string to_string() const
    {
        return boost::lexical_cast<std::string>(uuid_);
    }

    /**
     * Returns a null UUID with a full zero value.
     */
    static UUID null_uuid()
    {
        UUID u{};
        assert(u.uuid_.is_nil());
        return u;
    }

    static UUID random_uuid()
    {
        boost::uuids::uuid uuid = boost::uuids::random_generator()();
        return UUID(uuid);
    }

    bool operator<(const UUID &o) const
    {
        return uuid_ < o.uuid_;
    }

  private:
    boost::uuids::uuid uuid_;

    friend class odb::access;
};
}
