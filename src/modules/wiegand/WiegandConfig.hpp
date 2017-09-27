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
#include "hardware/GPIO.hpp"
#include "hardware/HardwareFwd.hpp"
#include "modules/wiegand/WiegandFwd.hpp"
#include "tools/db/database.hpp"
#include <chrono>

#pragma db model version(1, 1, open)

namespace Leosac
{
namespace Module
{
namespace Wiegand
{

/**
 * An instance of this class represents the configuration
 * of one Wiegand reader.
 */
#pragma db object callback(validation_callback)
struct WiegandReaderConfig
{
    WiegandReaderConfig()
        : id(0)
        , mode("SIMPLE_WIEGAND")
        , pin_timeout(2500)
        , pin_key_end('#')
        , enabled(true){};

    WiegandReaderConfig(const WiegandReaderConfig &) = default;

    /**
     * Returns the name of a device, or the empty string.
     */
    template <typename DevicePtrT>
    std::string device_name(const DevicePtrT &dev) const
    {
        if (dev)
            return dev->name();
        return "";
    }

    std::string gpio_high_name() const
    {
        return device_name(gpio_high_);
    }

    std::string gpio_low_name() const
    {
        return device_name(gpio_low_);
    }

    std::string green_led_name() const
    {
        return green_led;
        // return device_name(green_led);
    }

    std::string buzzer_name() const
    {
        return buzzer;
    }

    /**
     * ODB callback wrt database operation.
     */
    void validation_callback(odb::callback_event, odb::database &) const;

#pragma db id auto
    WiegandReaderConfigId id;

    std::string name;
    Hardware::GPIOPtr gpio_high_;
    Hardware::GPIOPtr gpio_low_;
    std::string green_led;
    std::string buzzer;
    std::string mode;

    std::chrono::milliseconds pin_timeout;
    char pin_key_end;

    /**
     * Is this reader configuration active?
     */
    bool enabled;

    /**
     * List of valid operation mode for a reader.
     */
    constexpr static std::array<const char *const, 8> valid_operation_modes{
        "SIMPLE_WIEGAND",
        "WIEGAND_PIN_4BITS",
        "WIEGAND_PIN_8BITS",
        "WIEGAND_PIN_BUFFERED",
        "WIEGAND_CARD_PIN_4BITS",
        "WIEGAND_CARD_PIN_8BITS",
        "WIEGAND_CARD_PIN_BUFFERED",
        "AUTODETECT"};
};

/**
 * Transient configuration object that stores the list of
 * reader use/configure.
 *
 * This object is either populated from the database or from
 * the XML configuration.
 **/
class WiegandConfig
{
  public:
    WiegandConfig() = default;

    void add_reader(WiegandReaderConfigPtr);
    const std::vector<WiegandReaderConfigPtr> &readers() const;

  private:
    std::vector<WiegandReaderConfigPtr> readers_;
};
}
}
}

#ifdef ODB_COMPILER
#include "hardware/GPIO.hpp"
#endif
