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

#include "tools/serializers/ScheduleSerializer.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/ScheduleMapping.hpp"
#include "tools/SingleTimeFrame.hpp"
#include "tools/log.hpp"
#include <boost/algorithm/string.hpp>

using namespace Leosac;
using namespace Leosac::Tools;

json ScheduleJSONSerializer::serialize(const Tools::ISchedule &in,
                                       const SecurityContext &)
{
    int tf_pos      = 0;
    json timeframes = json::array();
    for (const auto &tf : in.timeframes())
    {
        // Note that a SingleTimeFrame can only be enabled on 1 day.
        std::string start_time =
            std::to_string(tf.start_hour) + ':' + std::to_string(tf.start_min);
        std::string end_time =
            std::to_string(tf.end_hour) + ':' + std::to_string(tf.end_min);
        timeframes.push_back({{"id", tf_pos},
                              {"start-time", start_time},
                              {"end-time", end_time},
                              {"day", tf.day}});
        tf_pos++;
    }

    json json_mappings = json::array();
    for (const auto &mapping : in.mapping())
    {
        json json_mapping = {{"id", mapping->id()}, {"type", "schedule-mapping"}};
        json_mappings.push_back(json_mapping);
    }

    json serialized = {{"id", in.id()},
                       {"type", "schedule"},
                       {"attributes",
                        {{"version", in.odb_version()},
                         {"name", in.name()},
                         {"description", in.description()},
                         {"timeframes", timeframes}}},
                       {"relationships", {{"mapping", {{"data", json_mappings}}}}}};

    return serialized;
}

/**
 * Returns a hour/mine pair from a time string formatted like "hh:mm"
 */
static std::pair<int, int> parse_time(const std::string &tm)
{
    std::vector<std::string> splitted;
    boost::algorithm::split(splitted, tm, boost::algorithm::is_any_of(":"),
                            boost::algorithm::token_compress_on);
    if (splitted.size() != 2)
        throw LEOSACException(
            BUILD_STR("Timeframe start or end time is invalid: " << tm));

    return std::make_pair(std::stoi(splitted[0]), std::stoi(splitted[1]));
};

void ScheduleJSONSerializer::unserialize(Tools::ISchedule &out, const json &in,
                                         const SecurityContext &)
{
    using namespace Leosac::JSONUtil;

    out.name(extract_with_default(in, "name", out.name()));
    out.description(extract_with_default(in, "description", out.description()));

    out.clear_timeframes();
    for (const auto &tf_json : in.at("timeframes"))
    {
        std::pair<int, int> start_time = parse_time(tf_json.at("start-time"));
        std::pair<int, int> end_time   = parse_time(tf_json.at("end-time"));

        SingleTimeFrame tf(tf_json.at("day"), start_time.first, start_time.second,
                           end_time.first, end_time.second);
        out.add_timeframe(tf);
    }
}

std::string ScheduleJSONStringSerializer::serialize(const Tools::ISchedule &in,
                                                    const SecurityContext &sc)
{
    return ScheduleJSONSerializer::serialize(in, sc).dump(4);
}

void ScheduleJSONStringSerializer::unserialize(Tools::ISchedule &out,
                                               const std::string &in,
                                               const SecurityContext &sc)
{
    ScheduleJSONSerializer::unserialize(out, in, sc);
}
