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

#include "AccessPointSerializer.hpp"
#include "DoorSerializer.hpp"
#include "core/auth/IAccessPoint.hpp"
#include "tools/JSONUtils.hpp"

using namespace Leosac;
using namespace Leosac::Auth;

json AccessPointJSONSerializer::serialize(const Auth::IAccessPoint &ap,
                                          const SecurityContext &)
{
    // todo: type probably needs to be runtime-generated
    // based on the controller-module value
    json serialized = {{"id", ap.id()},
                       {"type", "access-point"},
                       {"attributes",
                        {
                            {"alias", ap.alias()},
                            {"description", ap.description()},
                            {"controller-module", ap.controller_module()},
                        }}};

    if (ap.door_id())
    {
        serialized["relationships"]
                  ["door"] = {{"data", {{"id", ap.door_id()}, {"type", "door"}}}};
    }
    return serialized;
}

void AccessPointJSONSerializer::unserialize(Auth::IAccessPoint &out, const json &in,
                                            const SecurityContext &)
{
    using namespace Leosac::JSONUtil;

    out.alias(extract_with_default(in, "alias", out.alias()));
    out.description(extract_with_default(in, "description", out.description()));
}

std::string AccessPointJSONStringSerializer::serialize(const Auth::IAccessPoint &in,
                                                       const SecurityContext &sc)
{
    return AccessPointJSONSerializer::serialize(in, sc).dump(4);
}

void AccessPointJSONStringSerializer::unserialize(Auth::IAccessPoint &out,
                                                  const std::string &in,
                                                  const SecurityContext &sc)
{
    auto tmp = json::parse(in);
    AccessPointJSONSerializer::unserialize(out, tmp, sc);
}
