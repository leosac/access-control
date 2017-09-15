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

#include "ZoneSerializer.hpp"
#include "core/GetServiceRegistry.hpp"
#include "core/auth/Door_odb.h"
#include "core/auth/Zone_odb.h"
#include "tools/JSONUtils.hpp"
#include "tools/service/ServiceRegistry.hpp"

using namespace Leosac;
using namespace Leosac::Auth;

json ZoneJSONSerializer::serialize(const Auth::IZone &zone, const SecurityContext &)
{
    json serialized = {{"id", zone.id()},
                       {"type", "zone"},
                       {"attributes",
                        {{"alias", zone.alias()},
                         {"description", zone.description()},
                         {"type", static_cast<int>(zone.type())}}}};

    json children = json::array();
    for (const Auth::ZoneLPtr &lazy_zone : zone.children())
    {
        json child_zone_info = {{"id", lazy_zone.object_id()}, {"type", "zone"}};
        children.push_back(child_zone_info);
    }

    json doors = json::array();
    for (const Auth::DoorLPtr &lazy_door : zone.doors())
    {
        json door_info = {{"id", lazy_door.object_id()}, {"type", "door"}};
        doors.push_back(door_info);
    }

    serialized["relationships"] = {{"children", {{"data", children}}},
                                   {"doors", {{"data", doors}}}};
    return serialized;
}

void ZoneJSONSerializer::unserialize(Auth::IZone &out, const json &in,
                                     const SecurityContext &)
{
    using namespace Leosac::JSONUtil;
    auto db = get_service_registry().get_service<DBService>()->db();

    out.alias(extract_with_default(in, "alias", out.alias()));
    out.description(extract_with_default(in, "description", out.description()));
    out.type(extract_with_default(in, "type", Zone::Type::LOGICAL));

    auto doors_ids = in.at("doors");
    out.clear_doors();
    for (const auto &door_id : doors_ids)
    {
        auto door = Auth::DoorLPtr(*db, door_id.get<Auth::DoorId>());
        out.add_door(door);
    }

    auto children_ids = in.at("children");
    out.clear_children();
    for (const auto &child_id : children_ids)
    {
        auto child = Auth::ZoneLPtr(*db, child_id.get<Auth::ZoneId>());
        out.add_child(child);
    }
}

std::string ZoneJSONStringSerializer::serialize(const Auth::IZone &in,
                                                const SecurityContext &sc)
{
    return ZoneJSONSerializer::serialize(in, sc).dump(4);
}

void ZoneJSONStringSerializer::unserialize(Auth::IZone &out, const std::string &in,
                                           const SecurityContext &sc)
{
    auto tmp = json::parse(in);
    ZoneJSONSerializer::unserialize(out, tmp, sc);
}
