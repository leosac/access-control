/*
    Copyright (C) 2014-2025 Leosac

    This file is part of Leosac Access Control.

    Leosac Access Control is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac Access Control is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once


#include "LibgpiodGPIO.hpp"
#include "core/UserSecurityContext.hpp"
#include <nlohmann/json.hpp>
#include "hardware/serializers/GPIOSerializer.hpp"
#include "tools/JSONUtils.hpp"

namespace Leosac {
namespace Module {
namespace Libgpiod {

struct LibgpiodGPIOSerializer {
    static nlohmann::json serialize(const LibgpiodGPIO &in, const SecurityContext &sc) {
    auto serialized = Leosac::Hardware::GPIOSerializer::serialize(in, sc);
        // Override type for clarity
        serialized["type"] = "libgpiod-gpio";
        // Add libgpiod-specific attributes
        serialized["attributes"]["device"] = in.device();
        serialized["attributes"]["offset"] = in.offset();
        serialized["attributes"]["interrupt-mode"] = in.interrupt_mode();
        return serialized;
    }
    static void unserialize(LibgpiodGPIO &out, const nlohmann::json &in, const SecurityContext &sc) {
        using namespace Leosac::JSONUtil;
        Leosac::Hardware::GPIOSerializer::unserialize(out, in, sc);
        if (in.contains("attributes")) {
            const auto& attr = in.at("attributes");
            out.device(extract_with_default(attr, "device", out.device()));
            out.offset(extract_with_default(attr, "offset", out.offset()));
            out.interrupt_mode(extract_with_default(attr, "interrupt-mode", out.interrupt_mode()));
        }
    }
};

}
}
}
