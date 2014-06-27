/**
 * \file agpiodevice.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief AGpioDevice class implementation
 */

#include "agpiodevice.hpp"
#include "gpio/gpio.hpp"
#include "exception/deviceexception.hpp"

AGpioDevice::AGpioDevice(const std::string& name, IGPIOProvider& gpioProvider)
:   _gpio(nullptr),
    _name(name),
    _gpioProvider(gpioProvider)
{}

const std::string& AGpioDevice::getName() const
{
    return (_name);
}

void AGpioDevice::serialize(ptree& node)
{
    ptree   gpioNode;

    gpioNode.put<int>("<xmlattr>.pin", _gpio->getPinNo());
    gpioNode.put<std::string>("<xmlattr>.direction", _startDirection);
    if (_startDirection == "out")
        gpioNode.put<bool>("<xmlattr>.value", _startValue);
    node.put_child("gpio", gpioNode);
}

void AGpioDevice::deserialize(const ptree& node)
{
    const ptree&    gpioNode = node.get_child("gpio");
    int             pin = gpioNode.get<int>("<xmlattr>.pin");

    _startDirection = gpioNode.get<std::string>("<xmlattr>.direction", "out");
    _startValue = gpioNode.get<bool>("<xmlattr>.value", false);
    if (!(_gpio = _gpioProvider.getGPIO(pin)))
        throw (DeviceException("could not get GPIO device"));
    _gpio->setDirection((_startDirection == "in") ? GPIO::Direction::In : GPIO::Direction::Out);
    if (_startDirection == "out")
        _gpio->setValue(_startValue);
}
