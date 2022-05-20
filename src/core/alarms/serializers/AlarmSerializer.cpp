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

#include "AlarmSerializer.hpp"
#include "core/SecurityContext.hpp"
#include "core/alarms/IAlarmEntry.hpp"
#include "tools/log.hpp"

namespace Leosac
{
namespace Alarms
{
namespace Serializer
{
json AlarmJSON::serialize(const Alarms::IAlarmEntry &in, const SecurityContext &sc)
{
  auto timestamp = boost::posix_time::to_time_t(in.timestamp());

  json serialized = {
        {"id", in.id()},
        {"type", "alarm-entry"},
        {"attributes",
         {
             {"name", in.alarm()},
             {"state", static_cast<uint8_t>(in.state())},
             {"severity", static_cast<uint8_t>(in.severity())},
             {"alarm-type", static_cast<uint8_t>(in.type())},
             {"timestamp", timestamp},
             {"reason", in.reason()},
             {"description", in.generate_description()},
         }}};

    if (in.author_id())
    {
        serialized["relationships"]["author"] = {
            {"data", {{"id", in.author_id()}, {"type", "user"}}}};
    }

    return serialized;
}
}
}
}
