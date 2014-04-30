/**
 * \file hwmanager.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief hardware managing class
 */

#include "hwmanager.hpp"

HWManager::HWManager() {}

HWManager::~HWManager() {}

void HWManager::start()
{
    _gpioManager.startPolling();
}

void HWManager::stop()
{
    _gpioManager.stopPolling();
}

WiegandInterface* HWManager::buildWiegandInterface(IWiegandListener* listener)
{
    return (new WiegandInterface(_gpioManager, listener));
}
