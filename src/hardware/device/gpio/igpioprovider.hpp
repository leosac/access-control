/**
 * \file igpioprovider.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief interface for gpio poller
 */

#ifndef IGPIOPROVIDER_HPP
#define IGPIOPROVIDER_HPP

class GPIO;

class IGPIOProvider
{
public:
    virtual ~IGPIOProvider() {}
    virtual GPIO*    getGPIO(int gpioNo) = 0;
};

#endif // IGPIOPROVIDER_HPP
