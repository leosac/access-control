/*
    Copyright (C) 2014-2017 Leosac

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


#include "hardware/serializers/DeviceSerializer.hpp"
#include "tools/JSONUtils.hpp"

namespace Leosac
{
namespace Hardware
{
json DeviceSerializer::serialize(const Hardware::Device &in, const SecurityContext &)
{
    json serialized = {{"id", in.id()},
                       {"type", "device"},
                       {"attributes",
                        {{"name", in.name()},
                         {"device_class", in.device_class()},
                         {"enabled", in.enabled()}}}};

    return serialized;
}

void DeviceSerializer::unserialize(Hardware::Device &out, const json &in,
                                   const SecurityContext &)
{
    using namespace JSONUtil;

    out.name(extract_with_default(in, "name", out.name()));
    out.enabled(extract_with_default(in, "enabled", out.enabled()));
}
}
}
