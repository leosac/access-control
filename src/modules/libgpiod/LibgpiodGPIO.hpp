/*
    Copyright (C) 2014-2025 Leosac

    This file is part of Leosac Access Control.

    Leosac Access Control is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac Access Control is distributed in the hope that it will be useful,
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
namespace Libgpiod
{
using json = nlohmann::json;
/**
 * Libgpiod Module GPIO descriptor.
 *
 * It inherits Hardware::GPIO and adds Libgpiod specific
 * data.
 */
#pragma db object callback(validation_callback) table("HARDWARE_GPIO_Libgpiod")
class LibgpiodGPIO : public Hardware::GPIO
{
  public:
    LibgpiodGPIO();

    std::string device() const { return device_; }
    void device(const std::string& dev) { device_ = dev; }

    int offset() const { return offset_; }
    void offset(int o) { offset_ = o; }

    std::string interrupt_mode() const { return interrupt_mode_; }
    void interrupt_mode(const std::string& m) { interrupt_mode_ = m; }

  private:
    /**
     * Device name (e.g., gpiochip0)
     */
    std::string device_;
    /**
     * Pin offset on the device
     */
    int offset_;
    /**
     * Interrupt mode ("none", "rising", "falling", "both")
     */
    std::string interrupt_mode_;

    friend odb::access;
};


}
}
}
