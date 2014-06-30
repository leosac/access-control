/**
 * \file button.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Button class implementation
 */

#include "button.hpp"
#include "gpio/gpio.hpp"
#include "exception/deviceexception.hpp"

Button::Button(const std::string& name, IGPIOProvider& gpioProvider)
:   AGpioDevice(name, gpioProvider)
{}

void Button::deserialize(const ptree& node)
{
    AGpioDevice::deserialize(node);
}

bool Button::isPressed() const
{
    return (_gpio->getValue() == false);
}
