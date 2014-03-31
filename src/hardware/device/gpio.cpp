/**
 * \file gpio.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief GPIO device class
 */

#include "gpio.hpp"

#include <fstream>

GPIO::GPIO(int pinNo, const std::string& path)
:   _pinNo(pinNo),
    _path(path)
{
    // TODO
}

GPIO::~GPIO()
{
    // TODO
}

int GPIO::getPinNo() const
{
    return (_pinNo);
}

int GPIO::getDirection() const
{
    // TODO
    return (0);
}

void GPIO::setDirection(int direction)
{
    // TODO
    static_cast<void>(direction);
}

int GPIO::getValue() const
{
    // TODO
    return (0);
}

void GPIO::setValue(int value)
{
    // TODO
    static_cast<void>(value);
}
