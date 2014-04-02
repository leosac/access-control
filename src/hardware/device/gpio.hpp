/**
 * \file gpio.hpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief GPIO device class
 */

#ifndef GPIO_HPP
#define GPIO_HPP

#include <fstream>
#include <string>

class GPIO
{
    static const std::string    ExportPath;
    static const std::string    UnexportPath;
    static const std::string    GpioPrefix;
    static const std::string    DirectionFilename;
    static const std::string    ValueFilename;

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
    GPIO(int pinNo);
    ~GPIO();

public:
    int                 getPinNo() const;
    const std::string&  getPath() const;
    int                 getDirection();
    void                setDirection(int direction);
    int                 getValue();
    void                setValue(int value);

private:
    bool    exists();
    void    exportGpio();
    void    unexportGpio();

private:
    const int           _pinNo;
    const std::string   _path;
    std::fstream        _directionFile;
    std::fstream        _valueFile;
};

#endif // GPIO_HPP
