/**
 * \file gpio.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief GPIO device class
 */

#include "gpio.hpp"

#include <fstream>

GPIO::GPIO(int pinNo)
:   _pinNo(pinNo)
{

}

GPIO::~GPIO()
{

}

int GPIO::getPinNo() const
{
    return (_pinNo);
}
