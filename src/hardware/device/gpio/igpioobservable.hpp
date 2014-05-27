/**
 * \file igpioobservable.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief interface for gpio poller
 */

#ifndef IGPIOOBSERVALBE_HPP
#define IGPIOOBSERVALBE_HPP

#include "gpio.hpp"

#include "igpiolistener.hpp"

class IGPIOObservable
{
public:
    typedef void (IGPIOObservable::*CallbackFn)(int, GPIO::EdgeMode);

public:
    virtual ~IGPIOObservable() {}
    virtual void    registerListener(IGPIOListener* listener, int gpioNo, GPIO::EdgeMode mode = GPIO::Rising) = 0;
};

#endif // IGPIOOBSERVALBE_HPP
