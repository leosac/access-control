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

#include "core/auth/Zone.hpp"

namespace Leosac
{
namespace Auth
{
Zone::Zone()
    : id_(0)
    , type_(Type::LOGICAL)
    , version_(0)
{
}

ZoneId Zone::id() const
{
    return id_;
}

std::string Zone::alias() const
{
    return alias_;
}

std::string Zone::description() const
{
    return description_;
}

void Zone::alias(const std::string &alias)
{
    alias_ = alias;
}

void Zone::description(const std::string &desc)
{
    description_ = desc;
}

IZone::Type Zone::type() const
{
    return type_;
}

void Zone::type(IZone::Type t)
{
    type_ = t;
}

std::vector<ZoneLPtr> Zone::children() const
{
    return children_;
}

std::vector<DoorLPtr> Zone::doors() const
{
    return doors_;
}

void Zone::clear_children()
{
    children_.clear();
}

void Zone::clear_doors()
{
    doors_.clear();
}

void Zone::add_door(DoorLPtr door)
{
    doors_.push_back(door);
}

void Zone::add_child(ZoneLPtr zone)
{
    children_.push_back(zone);
}
}
}
