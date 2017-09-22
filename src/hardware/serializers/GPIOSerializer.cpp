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


#include "hardware/serializers/GPIOSerializer.hpp"
#include "tools/JSONUtils.hpp"

namespace Leosac
{
namespace Hardware
{
json GPIOSerializer::serialize(const Hardware::GPIO &in, const SecurityContext &sc)
{
    json serialized = {{"id", in.id()},
                       {"type", "gpio"},
                       {"attributes",
                        {{"name", in.name()},
                         {"number", in.number()},
                         {"direction", static_cast<int>(in.direction())},
                         {"default-value", in.default_value()}}}};
    return serialized;
}

void GPIOSerializer::unserialize(Hardware::GPIO &out, const json &in,
                                 const SecurityContext &sc)
{
    using namespace JSONUtil;

    out.name(extract_with_default(in, "name", out.name()));
    out.number(extract_with_default(in, "number", out.number()));
    out.direction(extract_with_default(in, "direction", out.direction()));
    out.default_value(
        extract_with_default(in, "default-value", out.default_value()));
}
}
}
