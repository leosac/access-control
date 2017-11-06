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
#include "hardware/Buzzer_odb.h"
#include "hardware/GPIO_odb.h"
#include "hardware/HardwareService.hpp"
#include "hardware/LED_odb.h"
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
    serialized["attributes"]["mode"]      = in.mode;

    serialized["relationships"] = json{};

    auto hwd_service =
        get_service_registry().get_service<Hardware::HardwareService>();
    ASSERT_LOG(hwd_service, "No hardware service.");

    serialized["relationships"]["gpio-high"] =
        hwd_service->serialize_device_metadata(in.gpio_high_);
    serialized["relationships"]["gpio-low"] =
        hwd_service->serialize_device_metadata(in.gpio_low_);
    serialized["relationships"]["buzzer"] =
        hwd_service->serialize_device_metadata(in.buzzer_);
    serialized["relationships"]["green-led"] =
        hwd_service->serialize_device_metadata(in.green_led_);

    return serialized;
}

void WiegandReaderConfigSerializer::unserialize(WiegandReaderConfig &out,
                                                const json &in,
                                                const SecurityContext &sc)
{
    using namespace JSONUtil;
    Hardware::RFIDReaderSerializer::unserialize(out, in, sc);

    out.mode = extract_with_default(in, "mode", out.mode);
    auto db                      = get_service_registry().get_service<DBService>();

    // High GPIO
    Hardware::DeviceId device_id = extract_with_default(in, "gpio_high_id", UUID{});
    if (!device_id.is_nil())
    {
        out.gpio_high_ =
            odb::lazy_shared_ptr<Hardware::GPIO>(*db->db(), device_id).load();
    }

    // Low GPIO
    device_id = extract_with_default(in, "gpio_low_id", UUID{});
    if (!device_id.is_nil())
    {
        out.gpio_low_ =
            odb::lazy_shared_ptr<Hardware::GPIO>(*db->db(), device_id).load();
    }

    // Buzzer
    device_id = extract_with_default(in, "buzzer_id", UUID{});
    if (!device_id.is_nil())
    {
        out.buzzer_ =
            odb::lazy_shared_ptr<Hardware::Buzzer>(*db->db(), device_id).load();
    }

    // Green Led
    device_id = extract_with_default(in, "green_led_id", UUID{});
    if (!device_id.is_nil())
    {
        out.green_led_ =
            odb::lazy_shared_ptr<Hardware::LED>(*db->db(), device_id).load();
    }
}
}
}
}
