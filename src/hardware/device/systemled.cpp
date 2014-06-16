/**
 * \file systemled.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief SystemLed class implementation
 */

#include "systemled.hpp"

#include <fstream>
#include <thread>

#include "exception/deviceexception.hpp"
#include "tools/log.hpp"
#include "tools/unixfs.hpp"

using std::this_thread::sleep_for;
using std::chrono::milliseconds;

const std::string SystemLed::DevicePathPrefix = "/sys/class/leds/";

SystemLed::SystemLed(const std::string& name)
:   _name(name)
{}

const std::string& SystemLed::getName() const
{
    return (_name);
}

void SystemLed::serialize(ptree& node)
{
    node.put<std::string>("<xmlattr>.ledname", _ledName);
}

void SystemLed::deserialize(const ptree& node)
{
    _ledName = node.get<std::string>("<xmlattr>.ledname");
    _path = DevicePathPrefix + _ledName + '/';
    _brightnessFile = _path + "brightness";
    _triggerFile = _path + "trigger";
    _delayOnFile = _path + "delay_on";
    _delayOffFile = _path + "delay_off";

    test(); // FIXME

    setActiveTrigger("timer");
}

void SystemLed::blink()
{
    setBrightness(0);
    sleep_for(milliseconds(1));
    setBrightness(255);
}

int SystemLed::getBrightness() const
{
    return (UnixFs::readSysFsValue<int>(_brightnessFile));
}

void SystemLed::setBrightness(int value) const
{
    UnixFs::writeSysFsValue<int>(_brightnessFile, value);
}

std::string SystemLed::getActiveTrigger() const
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
            if (size < 3)
                throw (DeviceException("Invalid trigger return"));
            return (ret.substr(1, size - 2));
        }
    }
    throw (DeviceException("Invalid trigger return"));
}

void SystemLed::setActiveTrigger(const std::string& trigger) const
{
    UnixFs::writeSysFsValue<std::string>(_triggerFile, trigger);
}

int SystemLed::getDelayOn() const
{
    return (UnixFs::readSysFsValue<int>(_delayOnFile));
}

void SystemLed::setDelayOn(int value) const
{
    UnixFs::writeSysFsValue<int>(_delayOnFile, value);
}

int SystemLed::getDelayOff() const
{
    return (UnixFs::readSysFsValue<int>(_delayOffFile));
}

void SystemLed::setDelayOff(int value) const
{
    UnixFs::writeSysFsValue<int>(_delayOffFile, value);
}

void SystemLed::test()
{
    LOG() << "Trigger=" << getActiveTrigger();
    setActiveTrigger("timer");
    LOG() << "Trigger=" << getActiveTrigger();
}
