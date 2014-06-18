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

const std::string   LedMonitorModule::TypeStrings[IModuleProtocol::ActivityTypes] = {
    "system",
    "auth"
};

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
    for (auto led : _leds)
    {
        ptree& lednode = node.add("led", std::string());

        lednode.put("<xmlattr>.type", TypeStrings[static_cast<int>(led.first)]);
        lednode.put("<xmlattr>.device", led.second.deviceName);
    }
    _leds.clear();
}

void LedMonitorModule::deserialize(const ptree& node)
{
    for (const auto& v : node)
    {
        MonitorLed  newLed;
        std::string typeString;

        if (v.first != "led")
            continue;
        newLed.deviceName = v.second.get<std::string>("<xmlattr>.device");
        if (!(newLed.instance = dynamic_cast<SystemLed*>(_hwmanager.getDevice(newLed.deviceName))))
            throw (ModuleException("could not retrieve device \'" + newLed.deviceName + '\''));
        typeString = v.second.get<std::string>("<xmlattr>.type");
        for (int i = 0; i < IModuleProtocol::ActivityTypes; ++i)
        {
            if (TypeStrings[i] == typeString)
            {
                IModuleProtocol::ActivityType type = static_cast<IModuleProtocol::ActivityType>(i);
                if (_leds.count(type) > 0)
                    throw (ModuleException("led already registered for type \'" + typeString + '\''));
                newLed.instance->setActiveTrigger("none");
                newLed.instance->setBrightness(255);
                _leds.emplace(type, newLed);
                break;
            }
        }
    }
}

void LedMonitorModule::notify(IModuleProtocol::ActivityType type)
{
    if (!_leds.count(type))
        return;
    _leds.at(type).instance->blink();
}
