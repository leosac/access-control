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

#include "exception/ModelException.hpp"
#include "modules/wiegand/WiegandConfig_odb.h"
#include <odb/pgsql/query.hxx>

namespace Leosac
{
namespace Module
{
namespace Wiegand
{
constexpr std::array<const char *const, 8>
    WiegandReaderConfig::valid_operation_modes;

void WiegandConfig::add_reader(WiegandReaderConfigPtr reader)
{
    readers_.push_back(reader);
}

const std::vector<WiegandReaderConfigPtr> &WiegandConfig::readers() const
{
    return readers_;
}

void WiegandReaderConfig::validation_callback(odb::callback_event e,
                                              odb::database &db) const
{
    Device::validation_callback(e, db);

    if (e == odb::callback_event::post_update ||
        e == odb::callback_event::post_persist)
    {
        if (std::find(valid_operation_modes.begin(), valid_operation_modes.end(),
                      mode) == valid_operation_modes.end())
        {
            throw ModelException("data/attributes/mode",
                                 "Invalid wiegand-reader mode.");
        }
    }
}
}
}
}
