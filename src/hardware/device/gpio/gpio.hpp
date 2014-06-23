/**
 * \file gpio.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief GPIO device class
 */

#ifndef GPIO_HPP
#define GPIO_HPP

#include <string>

// NOTE This class WILL throw on kernels older than 2.6.33 due to the lack of edge mode support

class GPIO
{
    static const std::string    ExportPath;
    static const std::string    UnexportPath;
    static const std::string    GpioPathPrefix;
    static const std::string    DirectionFilename;
    static const std::string    ValueFilename;
    static const std::string    EdgeFilename;
    static const std::string    ActiveLowFilename;
    static const int            EdgeModes = 4;
    static const std::string    EdgeStrings[EdgeModes];

public:
    enum class Direction {
        In = 0,
        Out
    };
    enum class Value {
        Low = 0,
        High
    };
    enum class EdgeMode : int {
        None = 0,
        Rising,
        Falling,
        Both
    };

public:
    explicit GPIO(int pinNo/*, const std::string& sysfsName*/);
    ~GPIO();

    GPIO(const GPIO& other) = delete;
    GPIO& operator=(const GPIO& other) = delete;

public:
    int                 getPinNo() const;
    const std::string&  getPath() const;
    int                 getPollFd() const;

public:
    Direction   getDirection() const;
    void        setDirection(Direction direction) const;
    bool        getValue() const;
    void        setValue(bool value) const;
    void        setValue(Value value) const;
    bool        isActiveLow() const;
    void        setActiveLow(bool state) const;
    EdgeMode    getEdgeMode() const;
    void        setEdgeMode(EdgeMode mode) const;

private:
    bool    exists() const;
    void    exportGpio() const;
    void    unexportGpio() const;

private:
    const int           _pinNo;
    const std::string   _path;
    const std::string   _directionFile;
    const std::string   _valueFile;
    const std::string   _activeLowFile;
    const std::string   _edgeFile;
    int                 _pollFd;
};

#endif // GPIO_HPP
