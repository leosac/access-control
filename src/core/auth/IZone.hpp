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

#include "core/auth/AuthFwd.hpp"
#include <memory>
#include <string>

namespace Leosac
{
namespace Auth
{

/**
 * The interface for zones.
 *
 * A zone is a hierarchical container for doors and other zones.
 * It allows users to better organize their doors and allows
 * easier schedule management.
 *
 * A zone have one of two @ref Type and any number of doors and
 * child zones.
 *
 * There is a few rules to respect:
 *   - Cycles are forbidden in parent/children relationship.
 *   - While a zone can have multiple Type::LOGICAL parent, it can
 *     only have one Type::PHYSICAL parent.
 *
 */
class IZone : public std::enable_shared_from_this<IZone>
{
  public:
    /**
     * Zone's type
     */
    enum class Type
    {
        PHYSICAL = 0x00,
        LOGICAL  = 0x01
    };
    virtual ZoneId id() const = 0;

    virtual std::string alias() const       = 0;
    virtual std::string description() const = 0;
    virtual Type type() const               = 0;

    virtual void alias(const std::string &alias)      = 0;
    virtual void description(const std::string &desc) = 0;
    virtual void type(Type t)                         = 0;

    virtual void clear_children() = 0;
    virtual void clear_doors()    = 0;

    virtual void add_door(DoorLPtr door)  = 0;
    virtual void add_child(ZoneLPtr zone) = 0;

    /**
     * Retrieve the children zones.
     */
    virtual std::vector<ZoneLPtr> children() const = 0;

    /**
     * Retrieve the doors associated with the zones.
     */
    virtual std::vector<DoorLPtr> doors() const = 0;
};
}
}
