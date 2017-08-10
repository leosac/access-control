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
#include "tools/db/database.hpp"
#include <chrono>

#pragma db model version(1, 1, open)

namespace Leosac
{
namespace Module
{
namespace Wiegand
{
using WiegandConfigId = unsigned long;


#pragma db value
struct WiegandReaderConfig
{
    WiegandReaderConfig()
        : mode("SIMPLE_WIEGAND")
        , pin_timeout(2500)
        , pin_key_end('#')
        , enabled(true){};

    WiegandReaderConfig(const WiegandReaderConfig &) = default;

    std::string name;
    std::string gpio_high;
    std::string gpio_low;
    std::string green_led;
    std::string buzzer;
    std::string mode;

    std::chrono::milliseconds pin_timeout;
    char pin_key_end;

    /**
     * Is this reader configuration active?
     */
    bool enabled;
};

/**
 *
 * Database backed configuration for the WIEGAND module.
 **/
#pragma db object table("WIEGAND_Config")
class WiegandConfig
{
  public:
    WiegandConfig();

    WiegandConfigId id() const;

    void add_reader(WiegandReaderConfig);
    void clear_reader();
    const std::vector<WiegandReaderConfig> &readers() const;

  private:
#pragma db id auto
    WiegandConfigId id_;

#pragma db value_not_null id_column("readerconfig_id")
    std::vector<WiegandReaderConfig> readers_;

    friend class odb::access;
};
}
}
}
