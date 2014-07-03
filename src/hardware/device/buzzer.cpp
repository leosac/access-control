/**
 * \file buzzer.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Buzzer class implementation
 */

#include "buzzer.hpp"

#include <thread>

#include "gpio/gpio.hpp"
#include "exception/deviceexception.hpp"

Buzzer::Buzzer(const std::string& name, IGPIOProvider& gpioProvider)
:   _name(name),
    _gpioDevice(gpioProvider)
{}

const std::string& Buzzer::getName() const
{
    return (_name);
}

void Buzzer::serialize(ptree& node)
{
    _gpioDevice.serialize(node);
}

void Buzzer::deserialize(const ptree& node)
{
    _gpioDevice.deserialize(node);
    if (_gpioDevice.getGpio()->getDirection() != GPIO::Direction::Out)
        throw (DeviceException("Gpio direction must be OUT"));
}

void Buzzer::beep(unsigned int frequencyHz, unsigned int durationMs)
{
//     unsigned int period = 1000 / frequencyHz; FIXME
//     unsigned int loops = durationMs / period; FIXME

    static_cast<void>(frequencyHz);
    static_cast<void>(durationMs);

    std::thread thread([this] () // FIXME FIXME FIXME
    {
        unsigned int loops = 1000;

        for (unsigned int i = 0; i < loops; ++i)
        {
            _gpioDevice.getGpio()->setValue(GPIO::Value::High);
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            _gpioDevice.getGpio()->setValue(GPIO::Value::Low);
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    } );
    thread.detach();
}
