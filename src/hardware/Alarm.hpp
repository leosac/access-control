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

namespace Leosac
{
namespace Hardware
{
  /**
   *  Abstraction of an Alarm device attributes.
   */
#pragma db object callback(validation_callback) table("HARDWARE_ALARM")
class Alarm : public Device
{
  public:

    enum class AlarmSeverity : uint8_t
    {
      SEVERITY_LOWEST           = 0,
      SEVERITY_LOW              = 1,
      SEVERITY_NORMAL           = 2,
      SEVERITY_IMPORTANT        = 3,
      SEVERITY_CRITICAL         = 4
    };

    explicit Alarm();

    Alarm(const AlarmSeverity& severity);

    virtual ~Alarm() = default;

    virtual void validation_callback(odb::callback_event e, odb::database &) const;

    virtual void severity(const AlarmSeverity &severity);

    virtual AlarmSeverity severity() const;

    const GPIOPtr &gpio() const;

    void gpio(const GPIOPtr &gpio);

  private:
    AlarmSeverity severity_;

    /**
     * The underlying GPIO.
     */
    GPIOPtr gpio_;

    friend class odb::access;
};

}
}

#ifdef ODB_COMPILER
#include "hardware/GPIO.hpp"
#endif
