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

#include "hardware/Device.hpp"
#include "hardware/HardwareFwd.hpp"
#include <odb/callback.hxx>
#include <odb/core.hxx>

namespace Leosac
{
namespace Hardware
{

/**
 *  Abstraction of LED device attributes.
 */
#pragma db object callback(validation_callback) table("HARDWARE_LED")
class LED : public Device
{
  public:
    explicit LED();
    virtual ~LED() = default;

    void validation_callback(odb::callback_event e, odb::database &) const override;

    const GPIOPtr &gpio() const;

    void gpio(const GPIOPtr &gpio);

    int64_t default_blink_duration() const;

    void default_blink_duration(int64_t default_blink_duration);

    int64_t default_blink_speed() const;

    void default_blink_speed(int64_t default_blink_speed);

  private:
    int64_t default_blink_duration_;
    int64_t default_blink_speed_;

    /**
     * The underlying GPIO.
     */
    GPIOPtr gpio_;


  private:
    friend odb::access;
};
}
}

#ifdef ODB_COMPILER
#include "hardware/GPIO.hpp"
#endif
