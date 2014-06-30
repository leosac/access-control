/**
 * \file led.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Led class implementation
 */

#include "led.hpp"

#include <thread>
#include <chrono>

#include "hardware/device/gpio/gpio.hpp"
#include "exception/deviceexception.hpp"

Led::Led(const std::string& name, IGPIOProvider& gpioProvider)
:   AGpioDevice(name, gpioProvider)
{}

void Led::deserialize(const ptree& node)
{
    AGpioDevice::deserialize(node);
    if (_gpio->getDirection() != GPIO::Direction::Out)
        throw (DeviceException("Gpio direction must be OUT"));
    _gpio->setValue(false);
}

void Led::turnOn()
{
    _gpio->setValue(true);
}

void Led::turnOn(unsigned int durationMs)
{
    std::thread thread([this, durationMs] ()
    {
        turnOn();
        std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
        turnOff();
    } );
    thread.detach();
}

void Led::turnOff()
{
    _gpio->setValue(false);
}

void Led::toggle()
{
    _gpio->setValue(!_gpio->getValue());
}
