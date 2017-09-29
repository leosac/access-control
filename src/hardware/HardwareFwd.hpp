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

#include "tools/Uuid.hpp"

namespace Leosac
{
namespace Hardware
{
/**
 * An enumeration describing the class of the device.
 *
 * Devices are grouped together based on what they are.
 *
 * The DeviceClass represents the type of device that leosac
 * is aware of and can deal with. Modules provides implementation
 * for supporting existing DeviceClass.
 *
 * If new type of hardware would become supported (biometric reader),
 * a new entry in the DeviceClass enumeration should be added before
 * a module can provide proper support for the device.
 */
enum class DeviceClass
{
    UNKNOWN      = 0,
    GPIO         = 1,
    RFID_READERS = 2,
};

using DeviceId = UUID;

class GPIO;
using GPIOPtr = std::shared_ptr<GPIO>;
}
}
