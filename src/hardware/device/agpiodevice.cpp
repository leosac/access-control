/**
 * \file agpiodevice.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief AGpioDevice class implementation
 */

#include "agpiodevice.hpp"
#include "gpio/gpio.hpp"
#include "exception/deviceexception.hpp"

AGpioDevice::AGpioDevice(const std::string& name, IGPIOProvider& gpioProvider)
:   _gpio(nullptr),
    _name(name),
    _gpioProvider(gpioProvider),
    _gpioNo(0)
{}

const std::string& AGpioDevice::getName() const
{
    return (_name);
}

void AGpioDevice::serialize(ptree& node)
{
    node.put<int>("gpio", _gpioNo);
}

void AGpioDevice::deserialize(const ptree& node)
{
    _gpioNo = node.get<int>("gpio");
    if (!(_gpio = _gpioProvider.getGPIO(_gpioNo)))
        throw (DeviceException("could not get GPIO device"));
}
