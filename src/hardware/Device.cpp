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

#include "hardware/Device.hpp"
#include "HardwareService.hpp"
#include "exception/ModelException.hpp"
#include <boost/uuid/uuid_generators.hpp>
#include <core/GetServiceRegistry.hpp>

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
    , enabled_(true)
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


const std::string &Device::name() const
{
    return name_;
}

void Device::name(const std::string &name)
{
    name_ = name;
}

DeviceClass Device::device_class() const
{
    return device_class_;
}

void Device::device_class(DeviceClass d)
{
    device_class_ = d;
}

bool Device::enabled() const
{
    return enabled_;
}

void Device::enabled(bool e)
{
    enabled_ = e;
}

void Device::validation_callback(odb::callback_event e, odb::database &) const
{
    if (e == odb::callback_event::pre_persist ||
        e == odb::callback_event::pre_update)
    {
        // Make sure name is unique.
        auto hwd_service =
            get_service_registry().get_service<Hardware::HardwareService>();
        ASSERT_LOG(hwd_service, "No hardware service.");
        auto d = hwd_service->find_device_by_name(name());

        if (d && d->id() != id())
            throw ModelException("data/attributes/name", "Device name already used");
    }

    if (e == odb::callback_event::post_persist ||
        e == odb::callback_event::post_update)
    {
        if (name().empty())
        {
            throw ModelException("data/attributes/name",
                                 "Device name must be non empty");
        }
    }
}
}
}
