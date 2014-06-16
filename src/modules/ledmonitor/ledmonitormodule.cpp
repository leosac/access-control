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
#include "hardware/device/systemled.hpp"
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
    _led->setBrightness(0);
    node.put<std::string>("sysled", _ledName);
}

void LedMonitorModule::deserialize(const ptree& node)
{
    _ledName = node.get<std::string>("sysled");
    if (!(_led = dynamic_cast<SystemLed*>(_hwmanager.getDevice(_ledName))))
        throw (ModuleException("could not retrieve device \'" + _ledName + '\''));
    _led->setBrightness(255);
}

void LedMonitorModule::notify(IModuleProtocol::ActivityType type)
{
    static_cast<void>(type);

    LOG() << "Notified !";
    _led->blink();
}
