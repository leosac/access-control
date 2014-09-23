/**
 * \file igpioprovider.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief interface for gpio poller
 */

#ifndef IGPIOPROVIDER_HPP
#define IGPIOPROVIDER_HPP

#include "igpiolistener.hpp"

class GPIO;

/**
* Provides an interface for implementing GPIO control code.
*/
class IGPIOProvider
{
public:
    virtual ~IGPIOProvider() {}
    virtual GPIO*   getGPIO(int gpioNo) = 0;
    virtual void    registerListener(IGPIOListener* listener, GPIO* gpio) = 0;
    virtual void    unregisterListener(IGPIOListener* listener, GPIO* gpio) = 0;
};

#endif // IGPIOPROVIDER_HPP
