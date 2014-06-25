/**
 * \file igpioobservable.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief interface for gpio poller
 */

#ifndef IGPIOOBSERVABLE_HPP
#define IGPIOOBSERVABLE_HPP

#include "gpio.hpp"

#include "igpiolistener.hpp"

class IGPIOObservable
{
public:
    typedef void (IGPIOObservable::*CallbackFn)(int, GPIO::EdgeMode);

public:
    virtual ~IGPIOObservable() {}
    virtual void    registerListener(IGPIOListener* listener, int gpioNo) = 0;
};

#endif // IGPIOOBSERVABLE_HPP
