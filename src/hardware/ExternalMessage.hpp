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

#include "hardware/Device.hpp"
#include "hardware/HardwareFwd.hpp"
#include <odb/callback.hxx>
#include <odb/core.hxx>
#include <string>

namespace Leosac
{
namespace Hardware
{

/**
 *  Abstraction of an External Messaging device.
 *
 *  This is likely to be used by generic messaging libraries like MQTT or ZMQ
 *  itself but for external use (eg. notifying an external broker / IoT sytem
 *  for home automation etc...).
 */
#pragma db object callback(validation_callback) table("HARDWARE_ExternalMessage")
class ExternalMessage : public Device
{
  public:
    enum class Direction
    {
        Subscribe  = 0,
        Publish = 1
    };

    ExternalMessage();

    void validation_callback(odb::callback_event, odb::database &) const override;

    std::string subject() const;

    void subject(const std::string& subject);

    Direction direction() const;

    void direction(Direction direction);

    DeviceClass virtualtype() const;

    void virtualtype(DeviceClass virtualtype);

    std::string payload() const;

    void payload(const std::string& payload);

  protected:
    std::string subject_;
    Direction direction_;
    DeviceClass virtualtype_;
    std::string payload_;

  private:
    friend odb::access;
};
}
}
