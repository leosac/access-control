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
#include "DoorSerializer.hpp"
#include "tools/JSONUtils.hpp"

using namespace Leosac;
using namespace Leosac::Auth;

json DoorJSONSerializer::serialize(const Auth::IDoor &door,
                                   const SecurityContext &sc)
{
    json serialized = {
        {"id", door.id()},
        {"type", "door"},
        {"attributes",
         {
             {"alias", door.alias()}, {"description", door.description()},
         }}};
    return serialized;
}

void DoorJSONSerializer::unserialize(Auth::IDoor &out, const json &in,
                                     const SecurityContext &)
{
    using namespace Leosac::JSONUtil;

    out.alias(extract_with_default(in, "alias", out.alias()));
    out.description(extract_with_default(in, "description", out.description()));
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
