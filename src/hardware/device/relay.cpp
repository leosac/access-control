/**
 * \file relay.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Relay class implementation
 */

#include "relay.hpp"

#include "gpio/gpio.hpp"
#include "tools/log.hpp"
#include "exception/deviceexception.hpp"

Relay::Relay(const std::string& name, IGPIOProvider& gpioObservable)
:   AGpioDevice(name, gpioObservable),
    _isOpen(false)
{}

void Relay::serialize(ptree& node)
{
    AGpioDevice::serialize(node);
}

void Relay::deserialize(const ptree& node)
{
    AGpioDevice::deserialize(node);
}

void Relay::open(unsigned int durationMs)
{
    LOG() << "call";
    if (_openMutex.try_lock())
    {
        std::thread thread([this, durationMs] ()
        {
            _gpio->setValue(GPIO::Value::High);
            LOG() << "Relay HIGH";
            std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
            _gpio->setValue(GPIO::Value::Low);
            LOG() << "Relay LOW";
        } );
        thread.detach();
        _openMutex.unlock();
    }
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
