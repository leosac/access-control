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
:   _name(name),
    _gpioDevice(gpioObservable),
    _isOpen(false)
{}

const std::string& Relay::getName() const
{
    return (_name);
}

void Relay::serialize(ptree& node)
{
    _gpioDevice.serialize(node);
}

void Relay::deserialize(const ptree& node)
{
    _gpioDevice.deserialize(node);
    if (_gpioDevice.getGpio()->getDirection() != GPIO::Direction::Out)
        throw (DeviceException("Gpio direction must be OUT"));
}

void Relay::open(unsigned int durationMs)
{
    LOG() << "call";
    if (_openMutex.try_lock())
    {
        std::thread thread([this, durationMs] ()
        {
            _gpioDevice.getGpio()->setValue(GPIO::Value::High);
            LOG() << "Relay HIGH";
            std::this_thread::sleep_for(std::chrono::milliseconds(durationMs));
            _gpioDevice.getGpio()->setValue(GPIO::Value::Low);
            LOG() << "Relay LOW";
        } );
        thread.detach();
        _openMutex.unlock();
    }
}

void Relay::close()
{
    _gpioDevice.getGpio()->setValue(GPIO::Value::Low);
}

void Relay::setOpen(bool state)
{
    _gpioDevice.getGpio()->setValue(state);
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
