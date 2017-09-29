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
#include "tools/JSONUtils.hpp"

namespace Leosac
{
namespace Module
{
namespace Wiegand
{

static json serialize_gpio_metadata(const Hardware::GPIOPtr &gpio)
{
    auto hwd_service =
        get_service_registry().get_service<Hardware::HardwareService>();
    ASSERT_LOG(hwd_service, "No hardware service.");

    if (gpio)
    {
        std::string type = hwd_service->hardware_device_type(*gpio);
        json gpio_info   = {{"id", gpio->id()}, {"type", type}};
        return gpio_info;
    }
    return json{};
}

json WiegandReaderConfigSerializer::serialize(const WiegandReaderConfig &in,
                                              const SecurityContext &)
{
    json serialized = {{"id", in.id},
                       {"type", "wiegand-reader"},
                       {"attributes",
                        {{"name", in.name},
                         {"mode", in.mode},
                         {"enabled", in.enabled},
                         {"green_led", in.green_led_name()},
                         {"buzzer", in.buzzer_name()}}}};

    serialized["relationships"] = json{};
    serialized["relationships"]["gpio_high"] =
        serialize_gpio_metadata(in.gpio_high_);
    serialized["relationships"]["gpio_low"] = serialize_gpio_metadata(in.gpio_low_);

    return serialized;
}

void WiegandReaderConfigSerializer::unserialize(WiegandReaderConfig &out,
                                                const json &in,
                                                const SecurityContext &)
{
    using namespace JSONUtil;
    out.name      = extract_with_default(in, "name", out.name);
    out.mode      = extract_with_default(in, "mode", out.mode);
    out.enabled   = extract_with_default(in, "enabled", out.enabled);
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
