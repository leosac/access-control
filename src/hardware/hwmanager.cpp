/**
 * \file hwmanager.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief hardware managing class
 */

#include "hwmanager.hpp"

void HWManager::start()
{
    _gpioManager.startPolling();
}

void HWManager::stop()
{
    _gpioManager.stopPolling();
}

WiegandInterface* HWManager::buildWiegandInterface(IWiegandListener* listener, unsigned int hiGpioIdx, unsigned int loGpioIdx)
{
    return (new WiegandInterface(_gpioManager, listener, hiGpioIdx, loGpioIdx));
}
