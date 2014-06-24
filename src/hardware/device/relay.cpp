/**
 * \file relay.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Relay class implementation
 */

#include "relay.hpp"

#include "gpio/gpio.hpp"
#include "exception/deviceexception.hpp"

Relay::Relay(const std::string& name, IGPIOProvider& gpioObservable)
:   _name(name),
    _gpioProvider(gpioObservable)
{}

const std::string& Relay::getName() const
{
    return (_name);
}

void Relay::serialize(ptree& node)
{
    node.put<int>("gpio", _gpioNo);
}

void Relay::deserialize(const ptree& node)
{
    _gpioNo = node.get<int>("gpio");
    if (!(_gpio = _gpioProvider.getGPIO(_gpioNo)))
        throw (DeviceException("could not get GPIO device"));
    _gpio->setDirection(GPIO::Direction::Out);
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
