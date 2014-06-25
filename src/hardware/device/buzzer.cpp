/**
 * \file buzzer.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Buzzer class implementation
 */

#include "buzzer.hpp"

#include "gpio/gpio.hpp"
#include "exception/deviceexception.hpp"
#include <thread>

Buzzer::Buzzer(const std::string& name, IGPIOProvider& gpioProvider)
:   AGpioDevice(name, gpioProvider)
{}

void Buzzer::serialize(ptree& node)
{
    AGpioDevice::serialize(node);
}

void Buzzer::deserialize(const ptree& node)
{
    AGpioDevice::deserialize(node);
    _gpio->setDirection(GPIO::Direction::Out);

    // TEST
    beep(2000, 500);
}

void Buzzer::beep(unsigned int frequencyHz, unsigned int durationMs)
{
//     unsigned int period = 1000 / frequencyHz; FIXME
//     unsigned int loops = durationMs / period; FIXME

    static_cast<void>(frequencyHz);
    static_cast<void>(durationMs);

    unsigned int loops = 1000;

    for (unsigned int i = 0; i < loops; ++i)
    {
        _gpio->setValue(GPIO::Value::High);
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        _gpio->setValue(GPIO::Value::Low);
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
}
