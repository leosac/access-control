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

#pragma once

#include "hardware/HardwareFwd.hpp"
#include "tools/Uuid.hpp"
#include <cstdint>
#include <odb/callback.hxx>
#include <odb/core.hxx>
#include <string>

namespace Leosac
{
namespace Hardware
{

/**
 * Base class for hardware devices.
 *
 * Device is an abstraction around GPIOs, RFID-readers,
 * and possibly other devices.
 */
#pragma db object optimistic polymorphic
class Device
{
  public:
    Device();
    explicit Device(DeviceClass device_class);

    virtual ~Device() = default;

    UUID id() const;

    DeviceClass device_class() const;

    uint64_t odb_version() const;

  private:
#pragma db id
    DeviceId id_;

    DeviceClass device_class_;

#pragma db version
    uint64_t version_;

    friend odb::access;
};
}
}
