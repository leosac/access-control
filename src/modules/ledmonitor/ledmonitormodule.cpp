/**
 * \file ledmonitormodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief ledmonitor module class
 */

#include "ledmonitormodule.hpp"

#include <sstream>
#include <ctime>

#include "tools/log.hpp"

#include "hardware/device/led.hpp"
#include "exception/moduleexception.hpp"

LedMonitorModule::LedMonitorModule(ICore& core, const std::string& name)
:   _core(core),
    _hwmanager(core.getHWManager()),
    _name(name)
{}

const std::string& LedMonitorModule::getName() const
{
    return (_name);
}

IModule::ModuleType LedMonitorModule::getType() const
{
    return (ModuleType::ActivityMonitor);
}

void LedMonitorModule::serialize(ptree& node)
{
    ptree& properties = node.add("properties", std::string());

    properties.put<std::string>("led", _ledName);
}

void LedMonitorModule::deserialize(const ptree& node)
{
    ptree properties = node.get_child("properties");

    _ledName = properties.get<std::string>("led");
    if (!(_led = dynamic_cast<Led*>(_hwmanager.getDevice(_ledName))))
        throw (ModuleException("could not retrieve device \'" + _ledName + '\''));
}

void LedMonitorModule::blink()
{
    // TODO
}

