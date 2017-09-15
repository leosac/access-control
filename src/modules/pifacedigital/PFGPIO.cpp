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

#include "modules/pifacedigital/PFGPIO.hpp"
#include "hardware/serializers/GPIOSerializer.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/log.hpp"

namespace Leosac
{
namespace Module
{
namespace Piface
{

PFGPIO::PFGPIO()
    : hardware_address_(0)
{
}

json PFGPIOSerializer::serialize(const PFGPIO &in, const SecurityContext &sc)
{
    auto serialized = Hardware::GPIOSerializer::serialize(in, sc);
    // Now we override the type.
    ASSERT_LOG(serialized.at("type").is_string(),
               "Base GPIO serialization did something unexpected.");
    serialized["type"] = "pfdigital.gpio";

    serialized["attributes"]["hardware_address"] = in.hardware_address();
    return serialized;
}

void PFGPIOSerializer::unserialize(PFGPIO &out, const json &in,
                                   const SecurityContext &sc)
{
    using namespace JSONUtil;
    Hardware::GPIOSerializer::unserialize(out, in, sc);

    out.hardware_address(
        extract_with_default(in, "hardware_address", out.hardware_address()));
}
}
}
}
