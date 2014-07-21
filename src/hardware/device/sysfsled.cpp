/**
 * \file sysfsled.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief SysFsLed class implementation
 */

#include "sysfsled.hpp"

#include <fstream>
#include <thread>

#include "exception/deviceexception.hpp"
#include "tools/log.hpp"
#include "tools/unixfs.hpp"

const std::string SysFsLed::DevicePathPrefix = "/sys/class/leds/";

SysFsLed::SysFsLed(const std::string& name)
:   _name(name)
{}

const std::string& SysFsLed::getName() const
{
    return (_name);
}

void SysFsLed::serialize(ptree& node)
{
    setActiveTrigger(_startTrigger);
    node.put<std::string>("<xmlattr>.ledname", _ledName);
}

void SysFsLed::deserialize(const ptree& node)
{
    _ledName = node.get<std::string>("<xmlattr>.ledname");
    _path = DevicePathPrefix + _ledName + '/';
    _brightnessFile = _path + "brightness";
    _triggerFile = _path + "trigger";
    _delayOnFile = _path + "delay_on";
    _delayOffFile = _path + "delay_off";

    _startTrigger = getActiveTrigger();
    setBrightness(0);
    setActiveTrigger("none");
}

void SysFsLed::blink()
{
    setBrightness(0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    setBrightness(255);
    std::this_thread::sleep_for(std::chrono::milliseconds(80));
    setBrightness(0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    setBrightness(255);
}

int SysFsLed::getBrightness() const
{
    return (UnixFs::readSysFsValue<int>(_brightnessFile));
}

void SysFsLed::setBrightness(int value) const
{
    UnixFs::writeSysFsValue<int>(_brightnessFile, value);
}

std::string SysFsLed::getActiveTrigger() const
{
    std::ifstream   file(_triggerFile);
    std::string     ret;
    std::size_t     size;

    if (!file.good())
        throw (DeviceException("could not open " + _triggerFile + '\''));
    while (file >> ret)
    {
        if (ret[0] == '[')
        {
            size = ret.length();
            if (size < 3 || ret[size - 1] != ']')
                throw (DeviceException("Invalid trigger return \'" + ret + '\''));
            return (ret.substr(1, size - 2));
        }
    }
    throw (DeviceException("Invalid trigger return"));
}

void SysFsLed::setActiveTrigger(const std::string& trigger) const
{
    UnixFs::writeSysFsValue<std::string>(_triggerFile, trigger);
}

int SysFsLed::getDelayOn() const
{
    return (UnixFs::readSysFsValue<int>(_delayOnFile));
}

void SysFsLed::setDelayOn(int milliseconds) const
{
    UnixFs::writeSysFsValue<int>(_delayOnFile, milliseconds);
}

int SysFsLed::getDelayOff() const
{
    return (UnixFs::readSysFsValue<int>(_delayOffFile));
}

void SysFsLed::setDelayOff(int milliseconds) const
{
    UnixFs::writeSysFsValue<int>(_delayOffFile, milliseconds);
}
