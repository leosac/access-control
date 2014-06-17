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
#include "hardware/device/button.hpp"
#include "exception/moduleexception.hpp"

DoorModule::DoorModule(ICore& core, const std::string& name)
:   _core(core),
    _hwmanager(core.getHWManager()),
    _name(name),
    _doorButton(nullptr),
    _grantedLed(nullptr),
    _deniedLed(nullptr)
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
    for (std::size_t i = 0; i < 7; ++i)
    {
        ptree ptre;

        ptre.put<int>("<xmlattr>.idx", i);
        ptre.put<bool>("<xmlattr>.opened", _days[i].open);
        ptre.put<int>("<xmlattr>.start", _days[i].start);
        ptre.put<int>("<xmlattr>.end", _days[i].end);
        node.add_child("day", ptre);
    }
    node.put<std::string>("doorRelay", _config.doorRelay);
    node.put<std::string>("doorButton", _config.doorButton);
    node.put<std::string>("grantedLed", _config.grantedLed);
    node.put<std::string>("deniedLed", _config.deniedLed);
}

void DoorModule::deserialize(const ptree& node)
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
    _config.doorRelay = node.get<std::string>("doorRelay", "none");
    _config.doorButton = node.get<std::string>("doorButton", "none");
    _config.grantedLed = node.get<std::string>("grantedLed", "none");
    _config.deniedLed = node.get<std::string>("deniedLed", "none");

    loadDoorRelay();
    loadDoorButton();
    loadGrantedLed();
    loadDeniedLed();
}

bool DoorModule::isAuthRequired() const
{
    return (true); // FIXME
}

void DoorModule::open()
{
    if (_grantedLed)
        _grantedLed->blink(); // DEBUG
}

void DoorModule::loadDoorRelay()
{
    // TODO
}

void DoorModule::loadDoorButton()
{
    if (_config.doorButton == "none")
        return;
    if (!(_doorButton = dynamic_cast<Button*>(_hwmanager.getDevice(_config.doorButton))))
        throw (ModuleException("could not retrieve device \'" + _config.doorButton + '\''));
}

void DoorModule::loadGrantedLed()
{
    if (_config.grantedLed == "none")
        return;
    if (!(_grantedLed = dynamic_cast<Led*>(_hwmanager.getDevice(_config.grantedLed))))
        throw (ModuleException("could not retrieve device \'" + _config.grantedLed + '\''));
}

void DoorModule::loadDeniedLed()
{
    if (_config.deniedLed == "none")
        return;
    if (!(_deniedLed = dynamic_cast<Led*>(_hwmanager.getDevice(_config.deniedLed))))
        throw (ModuleException("could not retrieve device \'" + _config.deniedLed + '\''));
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
