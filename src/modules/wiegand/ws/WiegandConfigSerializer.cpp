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

#include "modules/wiegand/ws/WiegandConfigSerializer.hpp"
#include "core/GetServiceRegistry.hpp"
#include "hardware/GPIO_odb.h"
#include "hardware/HardwareService.hpp"
#include "hardware/serializers/RFIDReaderSerializer.hpp"
#include "tools/JSONUtils.hpp"

namespace Leosac
{
namespace Module
{
namespace Wiegand
{

json WiegandReaderConfigSerializer::serialize(const WiegandReaderConfig &in,
                                              const SecurityContext &sc)
{
    json serialized = Hardware::RFIDReaderSerializer::serialize(in, sc);
    // Now we override the type.
    ASSERT_LOG(serialized.at("type").is_string(),
               "Base GPIO serialization did something unexpected.");

    serialized["type"]                    = "wiegand-reader";
    serialized["attributes"]["green_led"] = in.green_led_name();
    serialized["attributes"]["buzzer"]    = in.buzzer_name();
    serialized["attributes"]["mode"]      = in.mode;

    serialized["relationships"] = json{};

    auto hwd_service =
        get_service_registry().get_service<Hardware::HardwareService>();
    ASSERT_LOG(hwd_service, "No hardware service.");

    serialized["relationships"]["gpio_high"] =
        hwd_service->serialize_device_metadata(in.gpio_high_);
    serialized["relationships"]["gpio_low"] =
        hwd_service->serialize_device_metadata(in.gpio_low_);

    return serialized;
}

void WiegandReaderConfigSerializer::unserialize(WiegandReaderConfig &out,
                                                const json &in,
                                                const SecurityContext &sc)
{
    using namespace JSONUtil;
    Hardware::RFIDReaderSerializer::unserialize(out, in, sc);

    out.mode      = extract_with_default(in, "mode", out.mode);
    out.green_led = extract_with_default(in, "green_led", out.green_led_name());
    out.buzzer    = extract_with_default(in, "buzzer", out.buzzer_name());

    // High GPIO
    Hardware::DeviceId gpio_id = extract_with_default(in, "gpio_high_id", UUID{});
    auto db                    = get_service_registry().get_service<DBService>();
    if (!gpio_id.is_nil())
    {
        out.gpio_high_ =
            odb::lazy_shared_ptr<Hardware::GPIO>(*db->db(), gpio_id).load();
    }

    // Low GPIO
    gpio_id = extract_with_default(in, "gpio_low_id", UUID{});
    if (!gpio_id.is_nil())
    {
        out.gpio_low_ =
            odb::lazy_shared_ptr<Hardware::GPIO>(*db->db(), gpio_id).load();
    }
}
}
}
}
