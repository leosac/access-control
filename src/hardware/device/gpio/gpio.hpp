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
    Direction   getDirection();
    void        setDirection(Direction direction);
    bool        getValue();
    void        setValue(bool state);
    bool        isActiveLow();
    void        setActiveLow(bool state);
    EdgeMode    getEdgeMode();
    void        setEdgeMode(EdgeMode mode);

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
    int                 _pollFd;
};

#endif // GPIO_HPP
