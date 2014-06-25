/**
 * \file relay.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Relay class implementation
 */

#include "relay.hpp"

#include "gpio/gpio.hpp"
#include "exception/deviceexception.hpp"

Relay::Relay(const std::string& name, IGPIOProvider& gpioObservable)
:   AGpioDevice(name, gpioObservable)
{}

void Relay::serialize(ptree& node)
{
    AGpioDevice::serialize(node);
}

void Relay::deserialize(const ptree& node)
{
    AGpioDevice::deserialize(node);
}

void Relay::open()
{
    _gpio->setValue(GPIO::Value::High);
}

void Relay::close()
{
    _gpio->setValue(GPIO::Value::Low);
}

void Relay::setOpen(bool state)
{
    _gpio->setValue(state);
}
