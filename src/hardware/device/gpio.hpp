/**
 * \file gpio.hpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief GPIO device class
 */

#ifndef GPIO_HPP
#define GPIO_HPP

#include <fstream>
#include <string>

// NOTE This class will throw on kernel older than 2.6.33 due to the lack of edge mode support

class GPIO
{
    static const std::string    ExportPath;
    static const std::string    UnexportPath;
    static const std::string    GpioPrefix;
    static const std::string    DirectionFilename;
    static const std::string    ValueFilename;
    static const std::string    EdgeFilename;
    static const std::string    ActiveLowFilename;

public:
    enum Direction {
        In = 0,
        Out = 1
    };
    enum Value {
        Low = 0,
        High = 1
    };
    enum EdgeMode {
        None = 0,
        Rising,
        Falling,
        Both
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
    void                setEdgeMode(int edgeMode);

private:
    bool    exists();
    void    exportGpio();
    void    unexportGpio();

private:
    const int           _pinNo;
    const std::string   _path;
    std::fstream        _directionFile;
    std::fstream        _valueFile;
    std::fstream        _edgeFile;
    std::fstream        _activeLowFile;
    int                 _valueFd; // NOTE Used for polling
};

#endif // GPIO_HPP
