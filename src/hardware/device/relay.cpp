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
    if (_openMutex.try_lock())
        _openThread = std::thread([this] ()
        {
            _gpio->setValue(GPIO::Value::High);
            std::this_thread::sleep_for(std::chrono::seconds(5));
            _gpio->setValue(GPIO::Value::Low);
            _openMutex.unlock();
        } );
}

void Relay::close()
{
    _gpio->setValue(GPIO::Value::Low);
}

void Relay::setOpen(bool state)
{
    _gpio->setValue(state);
}

bool Relay::isOpen()
{
    if (_openMutex.try_lock())
    {
        _openMutex.unlock();
        return (false);
    }
    return (true);
}
