/**
 * \file gpio.hpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief GPIO device class
 */

#ifndef GPIO_HPP
#define GPIO_HPP

#include <string>

class GPIO
{
public:
    enum Direction {
        Out = 0,
        In = 1,
    };
    enum Value {
        Low = 0,
        High = 1,
    };

public:
    GPIO(int pinNo, const std::string& path);
    ~GPIO();

public:
    int     getPinNo() const;
    int     getDirection() const;
    void    setDirection(int direction);
    int     getValue() const;
    void    setValue(int value);

private:
    const int           _pinNo;
    const std::string   _path;
};

#endif // GPIO_HPP
