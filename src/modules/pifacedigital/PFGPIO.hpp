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

#define ODB_NO_BASE_VERSION
#pragma db model version(1, 1)

#include "LeosacFwd.hpp"
#include "hardware/GPIO.hpp"
#include <nlohmann/json.hpp>

namespace Leosac
{
namespace Module
{

namespace Piface
{
using json = nlohmann::json;
/**
 * Piface Module GPIO descriptor.
 *
 * It inherits Hardware::GPIO and adds Piface specific
 * data.
 */
#pragma db object callback(validation_callback) table("HARDWARE_GPIO_Piface")
class PFGPIO : public Hardware::GPIO
{
  public:
    PFGPIO();

    uint8_t hardware_address() const
    {
        return hardware_address_;
    }

    void hardware_address(uint8_t addr)
    {
        hardware_address_ = addr;
    }

  private:
    /**
     * Hardware address of the piface digital device.
     *
     * This should be 0 when using a single piface.
     */
    uint8_t hardware_address_;

    friend odb::access;
};

struct PFGPIOSerializer
{
    static json serialize(const PFGPIO &in, const SecurityContext &sc);
    static void unserialize(PFGPIO &out, const json &in, const SecurityContext &sc);
};
}
}
}
