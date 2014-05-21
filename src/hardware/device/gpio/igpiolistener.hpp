/**
 * \file igpiolistener.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief interface for gpio event listeners
 */

#ifndef IGPIOLISTENER_HPP
#define IGPIOLISTENER_HPP

#include "gpio.hpp"

class IGPIOListener
{
public:
    virtual ~IGPIOListener() noexcept = default;
    virtual void    notify(int gpioNo) = 0; // NOTE An interrupt was caught
    virtual void    timeout() = 0; // NOTE Called when poll() times out
};

#endif // IGPIOLISTENER_HPP
