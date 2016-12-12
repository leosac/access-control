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

#include "AccessPointUpdateSerializer.hpp"
#include "UpdateSerializer.hpp"
#include "core/auth/AccessPointUpdate.hpp"
#include "tools/Conversion.hpp"
#include "tools/JSONUtils.hpp"

namespace Leosac
{
namespace update
{
json AccessPointUpdateJSONSerializer::serialize(const Auth::AccessPointUpdate &in,
                                                const SecurityContext &sc)
{
    json serialized(UpdateJSONSerializer::serialize(in, sc));

    serialized["type"]                          = "access-point-update";

    //fixme dynamically retrieve the correct type.
    serialized["relationships"]["access-point"] = {
        {"data", {{"id", in.access_point_id()}, {"type", "evoxs-access-point"}}}};

    return serialized;
}

std::string
AccessPointUpdateJSONStringSerializer::serialize(const Auth::AccessPointUpdate &in,
                                                 const SecurityContext &sc)
{
    return AccessPointUpdateJSONSerializer::serialize(in, sc).dump(4);
}
}
}
