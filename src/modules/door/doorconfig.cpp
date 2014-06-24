/**
 * \file doorconfig.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief DoorConfig class implementation
 */

#include "doorconfig.hpp"

#include <chrono>
#include <ctime>

#include "tools/log.hpp"

using std::chrono::system_clock;

void DoorConfig::serialize(ptree& node)
{
    for (std::size_t i = 0; i < 7; ++i)
    {
        ptree ptre;

        ptre.put<int>("<xmlattr>.idx", i);
        ptre.put<bool>("<xmlattr>.opened", _days[i].open);
        ptre.put<int>("<xmlattr>.start", _days[i].start);
        ptre.put<int>("<xmlattr>.end", _days[i].end);
        node.add_child("day", ptre);
    }
}

void DoorConfig::deserialize(const ptree& node)
{
    for (std::size_t i = 0; i < 7; ++i)
    {
        _days[i].open = false;
        _days[i].start = 0;
        _days[i].end = 24;
    }
    for (const auto& v : node)
    {
        if (v.first == "day")
        {
            int idx = v.second.get<int>("<xmlattr>.idx", 0);
            _days[idx].open = v.second.get<bool>("<xmlattr>.opened", true);
            _days[idx].start = v.second.get<int>("<xmlattr>.start", 0);
            _days[idx].end = v.second.get<int>("<xmlattr>.end", 24);
        }
    }
}

bool DoorConfig::isDoorOpenable() const
{
    system_clock::time_point    now = system_clock::now();
    std::time_t                 tt = system_clock::to_time_t(now);
    std::tm                     local_tm = *std::localtime(&tt);
    int                         day = local_tm.tm_wday;
    int                         hour = local_tm.tm_hour;

    if (!_days[day].open)
    {
        LOG() << "Day " << day << " closed";
        return (false);
    }
    if (!(hour >= _days[day].start && hour < _days[day].end))
    {
        LOG() << "Closed at this hour";
        return (false);
    }
    return (true);
}
