/**
 * \file hwmanager.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief hardware managing class
 */

#include "hwmanager.hpp"

#include <string>

#include "exception/deviceexception.hpp"
#include "tools/log.hpp"

#include "device/button.hpp"
#include "device/led.hpp"
#include "device/wiegandreader.hpp"

void HWManager::serialize(boost::property_tree::ptree& node)
{
    boost::property_tree::ptree hardware;

    for (auto& dev : _devices)
    {
        boost::property_tree::ptree& devNode = hardware.add("device", std::string());

        devNode.put("<xmlattr>.name", dev.first);
        devNode.put("<xmlattr>.type", dev.second.type);
        dev.second.instance->serialize(devNode);
        delete dev.second.instance;
    }
    node.put_child("hardware", hardware);
    _devices.clear();
}

void HWManager::deserialize(const boost::property_tree::ptree& node)
{
    boost::property_tree::ptree hardware = node.get_child("hardware");

    for (const auto& v : hardware)
    {
        if (v.first == "device")
        {
            Device                  dev;
            std::string             name(v.second.get<std::string>("<xmlattr>.name"));
            std::string             type(v.second.get<std::string>("<xmlattr>.type"));

            if (!(dev.instance = buildDevice(type, name)))
                throw (DeviceException("unable to load device of type \'" + type + '\''));
            dev.type = type;
            if (_devices.count(name) > 0)
                throw (DeviceException("device with duplicate name \'" + name + '\''));
            dev.instance->deserialize(v.second);
            _devices[name] = dev;
        }
    }
}

void HWManager::start()
{
#ifndef NO_HW
    _gpioManager.startPolling();
#endif
}

void HWManager::stop()
{
#ifndef NO_HW
    _gpioManager.stopPolling();
#endif
}

IDevice* HWManager::getDevice(const std::string& name)
{
    return (_devices.at(name).instance);
}

ISerializableDevice* HWManager::buildDevice(const std::string& type, const std::string& name)
{
    if (type == "button")
        return (new Button(name));
    else if (type == "wiegandreader")
        return (new WiegandReader(name, _gpioManager));
    else if (type == "led")
        return (new Led(name, _gpioManager));
    else
        return (nullptr);
}
