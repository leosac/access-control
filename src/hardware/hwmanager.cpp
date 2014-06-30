/**
 * \file hwmanager.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief hardware managing class
 */

#include "hwmanager.hpp"

#include <string>

#include "exception/deviceexception.hpp"
#include "tools/log.hpp"
#include "tools/leosac.hpp"

#include "device/button.hpp"
#include "device/led.hpp"
#include "device/systemled.hpp"
#include "device/wiegandreader.hpp"
#include "device/buzzer.hpp"
#include "device/relay.hpp"

void HWManager::serialize(ptree& node)
{
    ptree root;
    ptree platform;
    ptree devices;

    for (auto& alias : _gpioManager.getGpioAliases())
    {
        ptree& aliasNode = platform.add("gpioalias", alias.second);

        aliasNode.put("<xmlattr>.id", alias.first);
    }
    platform.put("<xmlattr>.name", _platform.name);
    for (auto& dev : _devices)
    {
        ptree& devNode = devices.add("device", std::string());

        devNode.put("<xmlattr>.name", dev.first);
        devNode.put("<xmlattr>.type", dev.second.type);
        dev.second.instance->serialize(devNode);
        delete dev.second.instance;
    }
    root.put_child("platform", platform);
    root.put_child("devices", devices);
    node.put_child("hardware", root);
    _devices.clear();
}

void HWManager::deserialize(const ptree& node)
{
    ptree root = node.get_child("hardware");
    ptree devices = root.get_child("devices");
    ptree platform = root.get_child("platform");

    _platform.name = platform.get<std::string>("<xmlattr>.name");
    LOG() << "current platform: " << _platform.name;

    for (const auto& v : platform)
    {
        if (v.first == "gpioalias")
            _gpioManager.setGpioAlias(v.second.get<int>("<xmlattr>.id"), v.second.data());
    }
    for (const auto& v : devices)
    {
        if (v.first == "device")
        {
            Device      dev;
            std::string name(v.second.get<std::string>("<xmlattr>.name"));
            std::string type(v.second.get<std::string>("<xmlattr>.type"));

            if (!(dev.instance = buildDevice(type, name)))
                throw (DeviceException("unable to load device of type \'" + type + '\''));
            dev.type = type;
            if (_devices.count(name) > 0)
                throw (DeviceException("device with duplicate name \'" + name + '\''));
            dev.instance->deserialize(v.second);
            _devices[name] = dev;
            LOG() << "Device " << name << " of type " << type << " loaded";
        }
    }
}

void HWManager::start()
{
    if (Leosac::Platform != Leosac::PlatformType::None)
        _gpioManager.startPolling();
}

void HWManager::stop()
{
    if (Leosac::Platform != Leosac::PlatformType::None)
        _gpioManager.stopPolling();
}

IDevice* HWManager::getDevice(const std::string& name)
{
    ISerializableDevice*    instance;

    try {
        instance = _devices.at(name).instance;
    }
    catch (const std::out_of_range& e) {
        throw (DeviceException("Bad device name: " + name));
    }
    return (instance);
}

const IHWManager::PlatformInfo& HWManager::getPlatformInfo() const
{
    return (_platform);
}

/*
 * NOTE This factory can be improved easily
 */
ISerializableDevice* HWManager::buildDevice(const std::string& type, const std::string& name)
{
    if (type == "button")
        return (new Button(name, _gpioManager));
    else if (type == "wiegandreader")
        return (new WiegandReader(name, _gpioManager));
    else if (type == "led")
        return (new Led(name, _gpioManager));
    else if (type == "systemled")
        return (new SystemLed(name));
    else if (type == "buzzer")
        return (new Buzzer(name, _gpioManager));
    else if (type == "relay")
        return (new Relay(name, _gpioManager));
    else
        return (nullptr);
}
