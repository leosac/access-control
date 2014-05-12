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
#ifdef NO_HW
    static_cast<void>(listener);
    static_cast<void>(hiGpioIdx);
    static_cast<void>(loGpioIdx);
    return (nullptr);
#else
    return (new WiegandInterface(_gpioManager, listener, hiGpioIdx, loGpioIdx));
#endif
}
