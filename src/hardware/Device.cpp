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

#include "hardware/Device.hpp"
#include <boost/uuid/uuid_generators.hpp>

namespace Leosac
{
namespace Hardware
{

Device::Device()
    : Device(DeviceClass::UNKNOWN)
{
}

Device::Device(DeviceClass device_class)
    : id_(UUID::random_uuid())
    , device_class_(device_class)
    , version_(0)
{
}

uint64_t Device::odb_version() const
{
    return version_;
}

UUID Device::id() const
{
    return id_;
}

DeviceClass Device::device_class() const
{
    return device_class_;
}
}
}
