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

    setTrigger("timer");
}

void SystemLed::blink()
{
    setBrightness(0);
    sleep_for(milliseconds(1));
    setBrightness(255);
}

int SystemLed::getBrightness() const
{
    std::fstream    file(_brightnessFile, std::ios::in);
    int             brightness;

    if (!file.good())
        throw (DeviceException("could not open " + _brightnessFile));
    file.seekp(0);
    file >> brightness;
    return (brightness);
}

void SystemLed::setBrightness(int value)
{
    std::fstream    file(_brightnessFile, std::ios::out);

    if (!file.good())
        throw (DeviceException("could not open " + _brightnessFile));
    file.seekp(0);
    file << value;
}

std::string SystemLed::getTrigger() const
{
    std::fstream    file(_triggerFile, std::ios::in);
    std::string     trigger;

    if (!file.good())
        throw (DeviceException("could not open " + _triggerFile));
    file.seekp(0);
    file >> trigger;
    return (trigger);
}

void SystemLed::setTrigger(const std::string& trigger)
{
    std::fstream    file(_triggerFile, std::ios::out);

    if (!file.good())
        throw (DeviceException("could not open " + _triggerFile));
    file.seekp(0);
    file << trigger;
}

void SystemLed::test()
{
    LOG() << "Trigger=" << getTrigger();
    setTrigger("timer");
    LOG() << "Trigger=" << getTrigger();
    setTrigger("none");
    LOG() << "Trigger=" << getTrigger();


    LOG() << "Brightness=" << getBrightness();
    setBrightness(0);
    LOG() << "Brightness=" << getBrightness();
    setBrightness(128);
    LOG() << "Brightness=" << getBrightness();
    setBrightness(255);
    LOG() << "Brightness=" << getBrightness();
}

int SystemLed::getDelayOn() const
{
    std::fstream    file(_delayOnFile, std::ios::in);
    int             delayOn;

    if (!file.good())
        throw (DeviceException("could not open " + _delayOnFile));
    file.seekp(0);
    file >> delayOn;
    return (delayOn);
}

void SystemLed::setDelayOn(int value)
{
    std::fstream    file(_delayOnFile, std::ios::out);

    if (!file.good())
        throw (DeviceException("could not open " + _delayOnFile));
    file.seekp(0);
    file << value;
}

int SystemLed::getDelayOff() const
{
    std::fstream    file(_delayOffFile, std::ios::in);
    int             delayOff;

    if (!file.good())
        throw (DeviceException("could not open " + _delayOffFile));
    file.seekp(0);
    file >> delayOff;
    return (delayOff);
}

void SystemLed::setDelayOff(int value)
{
    std::fstream    file(_delayOffFile, std::ios::out);

    if (!file.good())
        throw (DeviceException("could not open " + _delayOffFile));
    file.seekp(0);
    file << value;
}
