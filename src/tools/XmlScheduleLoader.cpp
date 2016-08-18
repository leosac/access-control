/*
    Copyright (C) 2014-2015 Islog

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

#include "XmlScheduleLoader.hpp"
#include "exception/configexception.hpp"
#include "log.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cassert>

using namespace Leosac::Tools;

int XmlScheduleLoader::week_day_to_int(const std::string &day)
{
    if (day == "sunday")
        return 0;
    if (day == "monday")
        return 1;
    if (day == "tuesday")
        return 2;
    if (day == "wednesday")
        return 3;
    if (day == "thursday")
        return 4;
    if (day == "friday")
        return 5;
    if (day == "saturday")
        return 6;
    ASSERT_LOG(0, "{" << day << "} is not a valid day of the week.");
    return -1;
}

bool XmlScheduleLoader::load(const boost::property_tree::ptree &schedules_tree)
{
    // make sure the map is clear.
    assert(schedules_.begin() == schedules_.end());
    for (const auto &sched : schedules_tree)
    {
        const std::string &node_name            = sched.first;
        const boost::property_tree::ptree &node = sched.second;

        if (node_name != "schedule")
        {
            std::string err = "Invalid configuration file content. Expected a XML "
                              "node named 'schedule' but found " +
                              node_name + " instead.";
            ERROR(err);
            return false;
        }
        extract_one(node);
    }
    return true;
}

bool XmlScheduleLoader::extract_one(const boost::property_tree::ptree &node)
{
    std::string schedule_name = node.get<std::string>("name");
    Schedule sched(schedule_name);

    // loop on all properties of the schedule.
    // those will be weekday and the <name> tag too.
    for (const auto &sched_data : node)
    {
        if (sched_data.first == "name") // we already got the name.
            continue;
        std::string start = sched_data.second.get<std::string>("start");
        std::string end   = sched_data.second.get<std::string>("end");
        std::vector<std::string> temp;

        boost::split(temp, start, boost::is_any_of(":"));
        if (temp.size() != 2)
            throw ModuleException("AuthFail schedule building error.");
        int start_hour = std::stoi(temp[0]);
        int start_min  = std::stoi(temp[1]);

        temp.clear();
        boost::split(temp, end, boost::is_any_of(":"));
        if (temp.size() != 2)
            throw ModuleException("AuthFail schedule building error.");
        int end_hour = std::stoi(temp[0]);
        int end_min  = std::stoi(temp[1]);

        Tools::SingleTimeFrame tf(week_day_to_int(sched_data.first), start_hour,
                                  start_min, end_hour, end_min);
        sched.add_timeframe(tf);
    }
    if (schedules_.count(schedule_name))
        NOTICE("A schedule with name " << schedule_name
                                       << " already exists. It will be overriden");

    schedules_[schedule_name] = sched;
    return true;
}

const std::map<std::string, Schedule> &XmlScheduleLoader::schedules() const
{
    return schedules_;
}
