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

#include "hardware/HardwareService.hpp"
#include "core/SecurityContext.hpp"
#include "hardware/Buzzer.hpp"
#include "hardware/Device_odb.h"
#include "hardware/LED.hpp"
#include "hardware/serializers/BuzzerSerializer.hpp"
#include "tools/db/OptionalTransaction.hpp"
#include <hardware/serializers/LEDSerializer.hpp>

namespace Leosac
{
namespace Hardware
{
HardwareService::HardwareService(const Leosac::DBServicePtr &dbservice)
    : dbservice_(dbservice)
{
    register_serializer<Hardware::Buzzer>(&BuzzerSerializer::serialize);
    register_serializer<Hardware::LED>(&LEDSerializer::serialize);
}

std::string
HardwareService::hardware_device_type(const Hardware::Device &device) const
{
    auto serialized = serialize(device, SystemSecurityContext::instance());
    ASSERT_LOG(serialized.at("type").is_string(),
               "Underlying serialization did something incorrect.");

    return serialized.at("type").get<std::string>();
}

json HardwareService::serialize_device_metadata(
    const Hardware::DevicePtr &device) const
{
    if (!device)
        return json{};

    std::string type  = hardware_device_type(*device);
    json dev_metadata = {{"id", device->id()}, {"type", type}};
    return dev_metadata;
}

DevicePtr HardwareService::find_device_by_name(const std::string &dev_name) const
{
    using Query = odb::query<Hardware::Device>;
    Query q(Query::name == dev_name);

    db::OptionalTransaction t(dbservice_->db()->begin());

    auto result = dbservice_->db()->query_one<Hardware::Device>(q);
    t.commit();
    return result;
}
}
}
