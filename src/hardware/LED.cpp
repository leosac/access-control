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

#include "hardware/LED.hpp"
#include "hardware/GPIO.hpp"

namespace Leosac
{
namespace Hardware
{

LED::LED()
    : Device(DeviceClass::LED)
{
}

void LED::validation_callback(odb::callback_event e, odb::database &db) const
{
    Device::validation_callback(e, db);
    if (gpio_)
        gpio_->validation_callback(e, db);
}
const GPIOPtr &LED::gpio() const
{
    return gpio_;
}

void LED::gpio(const GPIOPtr &gpio)
{
    gpio_ = gpio;
}

int64_t LED::default_blink_duration() const
{
    return default_blink_duration_;
}

void LED::default_blink_duration(int64_t default_blink_duration)
{
    default_blink_duration_ = default_blink_duration;
}

int64_t LED::default_blink_speed() const
{
    return default_blink_speed_;
}

void LED::default_blink_speed(int64_t default_blink_speed)
{
    default_blink_speed_ = default_blink_speed;
}
}
}
