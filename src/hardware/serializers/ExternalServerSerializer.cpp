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


#include "hardware/serializers/ExternalServerSerializer.hpp"
#include "hardware/serializers/DeviceSerializer.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/log.hpp"

namespace Leosac
{
namespace Hardware
{
json ExternalServerSerializer::serialize(const Hardware::ExternalServer &in,
                                     const SecurityContext &sc)
{
    json serialized = DeviceSerializer::serialize(in, sc);
    ASSERT_LOG(serialized.at("type").is_string(),
               "Base device serialization did something unexpected.");

    // Override object type
    serialized["type"] = "external-server";

    // Add External Message specific attributes
    serialized["attributes"]["host"]   = in.host();
    serialized["attributes"]["port"] = in.port();

    return serialized;
}

void ExternalServerSerializer::unserialize(Hardware::ExternalServer &out, const json &in,
                                       const SecurityContext &sc)
{
    using namespace JSONUtil;
    DeviceSerializer::unserialize(out, in, sc);

    out.host(extract_with_default(in, "host", out.host()));
    out.port(extract_with_default(in, "port", out.port()));
}
}
}
