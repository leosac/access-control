/**
 * \file hwmanager.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief hardware managing class
 */

#include "hwmanager.hpp"

void HWManager::start()
{
#ifndef NO_HW
    _gpioManager.startPolling();
#endif
}

void HWManager::stop()
{
#ifndef NO_HW
    _gpioManager.stopPolling();
#endif
}

WiegandInterface* HWManager::buildWiegandInterface(IWiegandListener* listener, unsigned int hiGpioIdx, unsigned int loGpioIdx)
{
#ifndef NO_HW
    return (new WiegandInterface(_gpioManager, listener, hiGpioIdx, loGpioIdx));
#endif
}
