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

#include "core/auth/Zone_odb.h"
#include "exception/ModelException.hpp"
#include "tools/log.hpp"

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

void Zone::validation_callback(odb::callback_event e, odb::database &) const
{
    if (e == odb::callback_event::post_update ||
        e == odb::callback_event::post_persist)
    {
        std::set<ZoneId> ids;
        ZoneValidator::validate(*this, ids);
    }
}

static void insert_enforce_unique(std::set<ZoneId> &zone_ids, ZoneId id)
{
    auto inserted = zone_ids.insert(id);
    if (!inserted.second)
    {
        // Already was inserted. We have a cycle.
        throw ModelException("",
                             "There cannot be cycle in parent/child relationship.");
    }
}

void ZoneValidator::validate(const Zone &z, std::set<ZoneId> &zone_ids)
{
    unsigned int physical_parent_count = 0;

    // Add current zone id to iterated zones. This will throw if zone
    // was already inserted.
    insert_enforce_unique(zone_ids, z.id());
    validate_type(z.type());

    for (auto &lazy_parent : z.parents_)
    {
        auto parent(lazy_parent.load());
        ASSERT_LOG(parent, "Failed to load object.");
        if (parent->type() == IZone::Type::PHYSICAL)
        {
            ++physical_parent_count;
        }
    }

    // Physical zone can have at most one physical parent.
    if (z.type() == IZone::Type::PHYSICAL && physical_parent_count > 1)
    {
        throw ModelException(
            "", "A physical zone cannot have more than one physical parent.");
    }
    // Logical zone cannot have physical parent.
    else if (z.type() == IZone::Type::LOGICAL && physical_parent_count != 0)
    {
        throw ModelException("",
                             "A logical zone cannot have physical zone as a parent");
    }

    // Recursively validate children.
    for (auto &lazy_children : z.children_)
    {
        auto child(lazy_children.load());
        ASSERT_LOG(child, "Failed to load object");
        ZoneValidator::validate(*child, zone_ids);
    }
}

void ZoneValidator::validate_type(IZone::Type value)
{
    switch (value)
    {
    case IZone::Type::PHYSICAL:
    case IZone::Type::LOGICAL:
        return;

    default:
        throw ModelException("data/attributes/type", "Invalid zone type.");
    }
}
}
}
