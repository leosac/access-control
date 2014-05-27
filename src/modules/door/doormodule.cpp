/**
 * \file doormodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief door module class
 */

#include "doormodule.hpp"

#include <sstream>
#include <chrono>
#include <ctime>
#include <thread>

#include "tools/log.hpp"

#include "hardware/device/gpio/gpio.hpp" // FIXME Debug

DoorModule::DoorModule(ICore& core, const std::string& name)
:   _listener(core),
    _hwmanager(core.getHWManager()),
    _name(name)
{}

void DoorModule::notify(const Event& event)
{
    std::istringstream  iss(event.message);
    std::string         uidstr;
    std::string         code;
    std::string         csn;

    iss >> uidstr;
    iss >> code;
    std::getline(iss, csn);
    if (code == "request")
    {
        if (!isDoorOpenable())
            _listener.notify(Event(uidstr + " denied", _name));
        else
            _listener.notify(Event(uidstr + " askauth " + csn, _name));
    }
    else if (code == "open")
        open();
}

const std::string& DoorModule::getName() const
{
    return (_name);
}

IModule::ModuleType DoorModule::getType() const
{
    return (ModuleType::Door);
}

void DoorModule::serialize(boost::property_tree::ptree& node)
{
    boost::property_tree::ptree& properties = node.add("properties", std::string());

    for (std::size_t i = 0; i < 7; ++i)
    {
        boost::property_tree::ptree ptre;

        ptre.put<int>("<xmlattr>.idx", i);
        ptre.put<bool>("<xmlattr>.opened", _days[i].open);
        ptre.put<int>("<xmlattr>.start", _days[i].start);
        ptre.put<int>("<xmlattr>.end", _days[i].end);
        properties.add_child("day", ptre);
    }
    if (_debugLed) // FIXME
        properties.put<int>("gpio", _debugLed->getPinNo());
    else
        properties.put<int>("gpio", 0);
}

void DoorModule::deserialize(boost::property_tree::ptree& node)
{
    boost::property_tree::ptree properties = node.get_child("properties");

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
            _days[idx].open = v.second.get<bool>("<xmlattr>.opened", false);
            _days[idx].start = v.second.get<int>("<xmlattr>.start", 0);
            _days[idx].end = v.second.get<int>("<xmlattr>.end", 24);
        }
    }
    _debugLed = _hwmanager.buildGPIO(properties.get<int>("gpio"));
    if (_debugLed) // FIXME
        _debugLed->setDirection(GPIO::Out);
}

void DoorModule::open()
{
    if (!_debugLed) // FIXME
        return ;
    _debugLed->setValue(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    _debugLed->setValue(false);
}

bool DoorModule::isDoorOpenable()
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t     tt = std::chrono::system_clock::to_time_t(now);
    std::tm         local_tm = *std::localtime(&tt);
    int             day = local_tm.tm_wday;
    int             hour = local_tm.tm_hour;

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
