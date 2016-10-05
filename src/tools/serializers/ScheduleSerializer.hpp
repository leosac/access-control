/*
    Copyright (C) 2014-2016 Islog

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

#include "tools/ISchedule.hpp"
#include "tools/Serializer.hpp"
#include <json.hpp>
#include <string>

namespace Leosac
{
namespace Tools
{
using json = nlohmann::json;

/**
 * Serialize schedules.
 */
struct ScheduleJSONSerializer
    : public Serializer<json, Tools::ISchedule, ScheduleJSONSerializer>
{
    static json serialize(const Tools::ISchedule &in, const SecurityContext &sc);

    static void unserialize(Tools::ISchedule &out, const json &in,
                            const SecurityContext &sc);
};

struct ScheduleJSONStringSerializer
    : public Serializer<std::string, Tools::ISchedule, ScheduleJSONStringSerializer>
{
    static std::string serialize(const Tools::ISchedule &in,
                                 const SecurityContext &sc);

    static void unserialize(Tools::ISchedule &out, const std::string &in,
                            const SecurityContext &sc);
};
}
}
