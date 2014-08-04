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

void Buzzer::beep(float frequencyHz, unsigned int durationMs)
{
    float           period = 1000.0f / frequencyHz;
    unsigned int    loops = durationMs / period;
    int             dt = static_cast<int>(period * 500.0f);
    
    std::thread thread([this, loops, dt] ()
    {
        std::chrono::system_clock::time_point   t = std::chrono::system_clock::now();

        for (unsigned int i = 0; i < loops; ++i)
        {
            t += std::chrono::microseconds(dt);
            _gpioDevice.getGpio()->setValue(GPIO::Value::High);
            std::this_thread::sleep_until(t);
            t += std::chrono::microseconds(dt);
            _gpioDevice.getGpio()->setValue(GPIO::Value::Low);
            std::this_thread::sleep_until(t);
        }
    } );
    thread.detach();
}
