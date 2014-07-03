/**
 * \file gpiodevice.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief GpioDevice class implementation
 */

#include "gpiodevice.hpp"
#include "gpio/gpio.hpp"
#include "exception/deviceexception.hpp"

GpioDevice::GpioDevice(IGPIOProvider& gpioProvider, const std::string& name)
:   _gpio(nullptr),
    _name(name),
    _gpioProvider(gpioProvider)
{}

void GpioDevice::serialize(ptree& node)
{
    ptree   gpioNode;

    gpioNode.put<int>("<xmlattr>.pin", _gpio->getPinNo());
    gpioNode.put<std::string>("<xmlattr>.direction", _startDirection);
    if (_startDirection == "out")
        gpioNode.put<bool>("<xmlattr>.value", _startValue);
    else
    {
        GPIO::EdgeMode  emode = _gpio->getEdgeMode();
        if (emode == GPIO::EdgeMode::Rising)
            gpioNode.put<std::string>("<xmlattr>.interruptmode", "rising");
        else if (emode == GPIO::EdgeMode::Falling)
            gpioNode.put<std::string>("<xmlattr>.interruptmode", "falling");
        else if (emode == GPIO::EdgeMode::Both)
            gpioNode.put<std::string>("<xmlattr>.interruptmode", "both");

    }
    node.put_child(_name, gpioNode);
}

void GpioDevice::deserialize(const ptree& node)
{
    const ptree&    gpioNode = node.get_child(_name);
    int             pin = gpioNode.get<int>("<xmlattr>.pin");
    std::string     interrupt = gpioNode.get<std::string>("<xmlattr>.interruptmode", "none");

    _startDirection = gpioNode.get<std::string>("<xmlattr>.direction", "out");
    _startValue = gpioNode.get<bool>("<xmlattr>.value", false);
    if (!(_gpio = _gpioProvider.getGPIO(pin)))
        throw (DeviceException("could not get GPIO device"));
    _gpio->setDirection((_startDirection == "in") ? GPIO::Direction::In : GPIO::Direction::Out);
    if (_startDirection == "out")
        _gpio->setValue(_startValue);
    else
    {
        if (interrupt == "rising")
            _gpio->setEdgeMode(GPIO::EdgeMode::Rising);
        else if (interrupt == "falling")
            _gpio->setEdgeMode(GPIO::EdgeMode::Falling);
        else if (interrupt == "both")
            _gpio->setEdgeMode(GPIO::EdgeMode::Both);
    }
}

GPIO* GpioDevice::getGpio()
{
    return (_gpio);
}

const GPIO* GpioDevice::getGpio() const
{
    return (_gpio);
}

void GpioDevice::startListening(IGPIOListener* listener)
{
    _gpioProvider.registerListener(listener, _gpio);
}
