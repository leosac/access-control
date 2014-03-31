/**
 * \file gpio.hpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief GPIO device class
 */

#ifndef GPIO_HPP
#define GPIO_HPP

class GPIO
{
public:
    GPIO(int pinNo);
    ~GPIO();

public:
    int getPinNo() const;

private:
    const int   _pinNo;
};

#endif // GPIO_HPP
