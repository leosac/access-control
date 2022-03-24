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


#include "hardware/serializers/BuzzerSerializer.hpp"
#include "core/GetServiceRegistry.hpp"
#include "hardware/Buzzer.hpp"
#include "hardware/GPIO_odb.h"
#include "hardware/HardwareService.hpp"
#include "hardware/serializers/DeviceSerializer.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/db/DBService.hpp"
#include "tools/log.hpp"

namespace Leosac
{
namespace Hardware
{
json BuzzerSerializer::serialize(const Hardware::Buzzer &in,
                                 const SecurityContext &sc)
{
    json serialized = DeviceSerializer::serialize(in, sc);
    ASSERT_LOG(serialized.at("type").is_string(),
               "Base device serialization did something unexpected.");

    // Override object type
    serialized["type"]                                 = "buzzer";
    serialized["attributes"]["default-blink-duration"] = in.default_blink_duration();
    serialized["attributes"]["default-blink-speed"]    = in.default_blink_speed();

    auto hwd_service =
        get_service_registry().get_service<Hardware::HardwareService>();
    ASSERT_LOG(hwd_service, "No hardware service.");

    serialized["relationships"]["gpio"] =
        hwd_service->serialize_device_metadata(in.gpio());

    return serialized;
}

void BuzzerSerializer::unserialize(Hardware::Buzzer &out, const json &in,
                                   const SecurityContext &sc)
{
    using namespace JSONUtil;
    DeviceSerializer::unserialize(out, in, sc);

    out.default_blink_duration(extract_with_default(in, "default-blink-duration",
                                                    out.default_blink_duration()));
    out.default_blink_speed(
        extract_with_default(in, "default-blink-speed", out.default_blink_speed()));

    Hardware::DeviceId gpio_id = extract_with_default(in, "gpio_id", UUID{});
    auto db                    = get_service_registry().get_service<DBService>();
    if (!gpio_id.is_nil())
    {
        out.gpio(odb::lazy_shared_ptr<Hardware::GPIO>(*db->db(), gpio_id).load());
    }
}
}
}
