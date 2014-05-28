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

void HWManager::serialize(boost::property_tree::ptree& node)
{
    boost::property_tree::ptree hardware;

    for (auto& dev : _devices)
    {
        boost::property_tree::ptree& devNode = hardware.add("device", std::string());

        devNode.put("<xmlattr>.name", dev.first);
        devNode.put("<xmlattr>.type", dev.second.type); // FIXME
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

            if (!(dev.instance = buildDevice(type)))
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

WiegandInterface* HWManager::buildWiegandInterface(IWiegandListener* listener, unsigned int hiGpioIdx, unsigned int loGpioIdx)
{
#ifdef NO_HW
    static_cast<void>(listener);
    static_cast<void>(hiGpioIdx);
    static_cast<void>(loGpioIdx);
    return (nullptr);
#else
    return (new WiegandInterface(_gpioManager, listener, hiGpioIdx, loGpioIdx));
#endif
}

GPIO* HWManager::buildGPIO(int idx)
{
#ifdef NO_HW
    static_cast<void>(idx);
    return (nullptr);
#else
    return (_gpioManager.getGPIO(idx));
#endif
}

ISerializableDevice* HWManager::buildDevice(const std::string& type)
{
// #ifdef NO_HW
//         return (nullptr);
// #endif

    if (type == "button")
        return (new Button);
    else
        return (nullptr);
}
