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

#include "core/auth/Door.hpp"
#include "AccessPoint_odb.h"
#include "DoorSerializer.hpp"
#include "core/auth/IAccessPoint.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/registry/ThreadLocalRegistry.hpp"

using namespace Leosac;
using namespace Leosac::Auth;

json DoorJSONSerializer::serialize(const Auth::IDoor &door, const SecurityContext &)
{
    json serialized = {
        {"id", door.id()},
        {"type", "door"},
        {"attributes",
         {
             {"alias", door.alias()}, {"description", door.description()},
         }}};

    if (door.access_point())
    {
        serialized["relationships"]["access-point"] = {
            {"data", {{"id", door.access_point()->id()}, {"type", "access-point"}}}};
    }
    return serialized;
}

void DoorJSONSerializer::unserialize(Auth::IDoor &out, const json &in,
                                     const SecurityContext &)
{
    using namespace Leosac::JSONUtil;

    out.alias(extract_with_default(in, "alias", out.alias()));
    out.description(extract_with_default(in, "description", out.description()));

    // Update the linked access point.
    Auth::AccessPointId current_ap_id = 0;
    if (out.access_point())
        current_ap_id = out.access_point()->id();

    Auth::AccessPointId new_ap_id =
        extract_with_default(in, "access_point_id", current_ap_id);
    if (new_ap_id != current_ap_id)
    {
        if (new_ap_id)
        {
            DBPtr dbptr =
                ThreadLocalRegistry::get<DBPtr>(ThreadLocalRegistry::DATABASE);
            Auth::AccessPointLPtr new_access_point(*dbptr, new_ap_id);
            out.access_point(new_access_point.load());
        }
        else
        {
            out.access_point(nullptr);
        }
    }
}

std::string DoorJSONStringSerializer::serialize(const Auth::IDoor &in,
                                                const SecurityContext &sc)
{
    return DoorJSONSerializer::serialize(in, sc).dump(4);
}

void DoorJSONStringSerializer::unserialize(Auth::IDoor &out, const std::string &in,
                                           const SecurityContext &sc)
{
    auto tmp = json::parse(in);
    DoorJSONSerializer::unserialize(out, tmp, sc);
}
