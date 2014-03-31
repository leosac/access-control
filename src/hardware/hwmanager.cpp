/**
 * \file hwmanager.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief hardware managing class
 */

#include "hwmanager.hpp"

#include "device/gpiomanager.hpp"

HWManager::HWManager()
:   _gpioManager(nullptr)
{
    // FIXME throws
    _gpioManager = new GPIOManager;
}

HWManager::~HWManager()
{}

GPIO* HWManager::reserveGPIO(int id)
{
    return (_gpioManager->reserve(id));
}
