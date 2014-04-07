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
    static const int            PollTimeoutDelayMs = 500;
    static const int            EdgeModes = 4;
    static const std::string    EdgeStrings[EdgeModes];

public:
    enum Direction {
        In = 0,
        Out
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

public:
    Direction           getDirection();
    void                setDirection(Direction direction);
    bool                getValue();
    void                setValue(bool state);
    bool                isActiveLow();
    void                setActiveLow(bool state);
    EdgeMode            getEdgeMode();
    void                setEdgeMode(EdgeMode mode);
    void                startPolling();

private:
    bool    exists();
    void    exportGpio();
    void    unexportGpio();

private:
    const int           _pinNo;
    const std::string   _path;
    const std::string   _directionFile;
    const std::string   _valueFile;
    const std::string   _activeLowFile;
    const std::string   _edgeFile;
};

#endif // GPIO_HPP
