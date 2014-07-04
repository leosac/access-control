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
:   _name(name),
    _gpioDevice(gpioProvider),
    _count(0)
{}

const std::string& Led::getName() const
{
    return (_name);
}

void Led::serialize(ptree& node)
{
    _gpioDevice.serialize(node);
}

void Led::deserialize(const ptree& node)
{
    _gpioDevice.deserialize(node);
    if (_gpioDevice.getGpio()->getDirection() != GPIO::Direction::Out)
        throw (DeviceException("Gpio direction must be OUT"));
}

void Led::turnOn()
{
    _gpioDevice.getGpio()->setValue(true);
}

void Led::turnOn(unsigned int durationMs)
{
    std::thread thread([this, durationMs] ()
    {
        if (!_count)
            turnOn();
        ++_count;
        std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
        --_count;
        if (!_count)
            turnOff();
    } );
    thread.detach();
}

void Led::turnOff()
{
    _gpioDevice.getGpio()->setValue(false);
}

void Led::toggle()
{
    _gpioDevice.getGpio()->setValue(!_gpioDevice.getGpio()->getValue());
}
