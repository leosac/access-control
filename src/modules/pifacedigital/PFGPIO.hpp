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
#include <json.hpp>

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
#pragma db object callback(validation_callback)
class PFGPIO : public Hardware::GPIO
{
  public:
    PFGPIO();

    int hardware_address() const
    {
        return hardware_address_;
    }

    void hardware_address(int addr)
    {
        hardware_address_ = addr;
    }

  private:
    // Not used for now. Maybe later when supporting
    // stacked piface. Really a dummy data for now.
    int hardware_address_;

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
