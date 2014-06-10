/**
 * \file doormodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief door module class
 */

#include "doormodule.hpp"

#include <sstream>
#include <chrono>
#include <ctime>

#include "tools/log.hpp"

#include "hardware/device/gpio/gpio.hpp" // FIXME Debug
#include "hardware/device/led.hpp"
#include "exception/moduleexception.hpp"

DoorModule::DoorModule(ICore& core, const std::string& name)
:   _core(core),
    _hwmanager(core.getHWManager()),
    _name(name)
{}

const std::string& DoorModule::getName() const
{
    return (_name);
}

IModule::ModuleType DoorModule::getType() const
{
    return (ModuleType::Door);
}

void DoorModule::serialize(ptree& node)
{
    ptree& properties = node.add("properties", std::string());

    for (std::size_t i = 0; i < 7; ++i)
    {
        ptree ptre;

        ptre.put<int>("<xmlattr>.idx", i);
        ptre.put<bool>("<xmlattr>.opened", _days[i].open);
        ptre.put<int>("<xmlattr>.start", _days[i].start);
        ptre.put<int>("<xmlattr>.end", _days[i].end);
        properties.add_child("day", ptre);
    }
    properties.put<std::string>("grantedLed", _grantedLedName);
}

void DoorModule::deserialize(const ptree& node)
{
    ptree properties = node.get_child("properties");

    for (std::size_t i = 0; i < 7; ++i)
    {
        _days[i].open = false;
        _days[i].start = 0;
        _days[i].end = 24;
    }
    for (const auto& v : properties)
    {
        if (v.first == "day")
        {
            int idx = v.second.get<int>("<xmlattr>.idx", 0);
            _days[idx].open = v.second.get<bool>("<xmlattr>.opened", true);
            _days[idx].start = v.second.get<int>("<xmlattr>.start", 0);
            _days[idx].end = v.second.get<int>("<xmlattr>.end", 24);
        }
    }
    _grantedLedName = properties.get<std::string>("grantedLed");
    if (!(_grantedLed = dynamic_cast<Led*>(_hwmanager.getDevice(_grantedLedName))))
        throw (ModuleException("could not retrieve device \'" + _grantedLedName + '\''));
}

void DoorModule::open()
{
    _grantedLed->blink(); // DEBUG
}

bool DoorModule::isDoorOpenable()
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
