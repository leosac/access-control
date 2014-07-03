/**
 * \file button.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Button class implementation
 */

#include "button.hpp"
#include "gpio/gpio.hpp"
#include "exception/deviceexception.hpp"

Button::Button(const std::string& name, IGPIOProvider& gpioProvider)
:   _name(name),
    _gpioDevice(gpioProvider)
{}

const std::string& Button::getName() const
{
    return (_name);
}

void Button::serialize(ptree& node)
{
    _gpioDevice.serialize(node);
}

void Button::deserialize(const ptree& node)
{
    _gpioDevice.deserialize(node);
    if (_gpioDevice.getGpio()->getDirection() != GPIO::Direction::In)
        throw (DeviceException("Gpio direction must be IN"));
}

bool Button::isPressed() const
{
    return (_gpioDevice.getGpio()->getValue() == false);
}
