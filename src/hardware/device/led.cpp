/**
 * \file led.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Led class implementation
 */

#include "led.hpp"

#include <thread>

#include "hardware/device/gpio/gpio.hpp"
#include "exception/deviceexception.hpp"

Led::Led(const std::string& name, IGPIOProvider& gpioProvider)
:   _name(name),
    _gpioProvider(gpioProvider)
{}

const std::string& Led::getName() const
{
    return (_name);
}

void Led::serialize(ptree& node)
{
    node.put<int>("gpio", _gpioNo);
}

void Led::deserialize(const ptree& node)
{
    _gpioNo = node.get<int>("gpio");
    if (!(_gpio = _gpioProvider.getGPIO(_gpioNo)))
        throw (DeviceException("could not get GPIO device"));
}

void Led::blink()
{
    _gpio->setValue(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    _gpio->setValue(false);
}
