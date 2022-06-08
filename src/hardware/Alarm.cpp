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

#include "hardware/Alarm.hpp"
#include "hardware/Alarm_odb.h"
#include "hardware/GPIO.hpp"

namespace Leosac
{
namespace Hardware
{

Alarm::Alarm()
    : Device(DeviceClass::ALARM)
    , severity_(AlarmSeverity::SEVERITY_NORMAL)
{
}

Alarm::Alarm(const Alarm::AlarmSeverity& severity)
    : Device(DeviceClass::ALARM),
    severity_(severity)
{
}

Alarm::AlarmSeverity Alarm::severity() const
{
    return severity_;
}

void Alarm::severity(const Alarm::AlarmSeverity& severity)
{
    severity_ = severity;
}

const GPIOPtr &Alarm::gpio() const
{
    return gpio_;
}

void Alarm::gpio(const GPIOPtr &gpio)
{
    gpio_ = gpio;
}

void Alarm::validation_callback(odb::callback_event e, odb::database &db) const
{
    Device::validation_callback(e, db);
}
}
}
