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
#include <cstdint>
#include <odb/callback.hxx>
#include <odb/core.hxx>
#include <string>

namespace Leosac
{
namespace Hardware
{

/**
 *  Abstraction of a RFID Reader device.
 *
 *  For now we have no attributes. Probably will change
 *  when WiegandReader module works correctly with database.
 *  todo: move some common attributes here
 */
#pragma db object callback(validation_callback) table("HARDWARE_RFIDReader")
class RFIDReader : public Device
{
  public:
    RFIDReader();

    void validation_callback(odb::callback_event, odb::database &) const override;

    friend odb::access;
};
}
}
