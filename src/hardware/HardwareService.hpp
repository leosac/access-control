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

#pragma once

#include "GPIO.hpp"
#include "LeosacFwd.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/db/db_fwd.hpp"
#include "tools/serializers/ExtensibleSerializer.hpp"

namespace Leosac
{
namespace Hardware
{
/**
 * Database aware Hardware Service.
 *
 * @note This is a core service that requires a database
 * to function. Therefore it'll be available only if Leosac is
 * configured to use a database.
 */
class HardwareService
    : public ExtensibleSerializer<json, Hardware::Device, const SecurityContext &>
{
  public:
    explicit HardwareService(const Leosac::DBServicePtr &dbservice);

    DevicePtr find_device_by_name(const std::string &dev_name) const;

    /**
     * Return the name of real type of a device.
     */
    std::string hardware_device_type(const Hardware::Device &device) const;

    /**
     * Serialize as json the metadata of a device.
     *
     * This is useful when serializer when to describe a relationship.
     * @param device
     * @return json object looking like: {"id": DEVICE_UUID, "type": DEVICE_TYPE}
     */
    json serialize_device_metadata(const Hardware::DevicePtr &device) const;

  private:
    DBServicePtr dbservice_;
};
}
}
