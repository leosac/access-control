/**
 * \file igpiolistener.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief interface for gpio event listeners
 */

#ifndef IGPIOLISTENER_HPP
#define IGPIOLISTENER_HPP

#include "gpio.hpp"

/**
* Interface for object registered to a GPIOProvider.
*/
class IGPIOListener
{
public:
    virtual ~IGPIOListener() {}

    /**
    * Interrupt on GPIO number gpioNo detected.
    */
    virtual void    notify(int gpioNo) = 0;

    /**
    * The GPIOManager poll() call has timed out.
    */
    virtual void    timeout() = 0;
};

#endif // IGPIOLISTENER_HPP
