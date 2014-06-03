/**
 * \file led.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Led class implementation
 */

#include "led.hpp"

#include <thread>

#include "hardware/device/gpio/gpio.hpp"
#include "exception/moduleexception.hpp"

Led::Led(IGPIOProvider& gpioProvider)
:   _gpioProvider(gpioProvider)
{}

IDevice::DeviceType Led::getType() const
{
    return (DeviceType::Led);
}

void Led::serialize(boost::property_tree::ptree& node)
{
    node.put<int>("gpio", _gpioNo);
}

void Led::deserialize(const boost::property_tree::ptree& node)
{
    _gpioNo = node.get<int>("gpio");
    if (!(_gpio = _gpioProvider.getGPIO(_gpioNo)))
        throw (ModuleException("could not get GPIO device"));
}

void Led::blink()
{
    _gpio->setValue(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    _gpio->setValue(false);
}
