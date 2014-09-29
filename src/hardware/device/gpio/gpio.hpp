/**
 * \file gpio.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief GPIO device class
 */

#ifndef GPIO_HPP
#define GPIO_HPP

#include <string>

/**
* Representation of a GPIO pin. Modifying its state require writing to SYSFS.
*/
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
    explicit GPIO(int pinNo, const std::string& sysfsName);

    /**
    * This constructor is used by PFDigitalGPIO
    */
    explicit GPIO(int pinNo);
    ~GPIO();

    GPIO(const GPIO& other) = delete;
    GPIO& operator=(const GPIO& other) = delete;

public:
    virtual int                 getPinNo() const;
    virtual const std::string&  getPath() const;
    virtual int                 getPollFd() const;

public:
    virtual Direction   getDirection() const;
    virtual void        setDirection(Direction direction) const;
    virtual bool        getValue() const;
    virtual void        setValue(bool value) const;
    virtual void        setValue(Value value) const;
    virtual bool        isActiveLow() const;
    virtual void        setActiveLow(bool state) const;
    virtual bool        hasInterruptsSupport() const;
    virtual EdgeMode    getEdgeMode() const;
    virtual void        setEdgeMode(EdgeMode mode) const;

private:
    bool    exists() const;
    void    exportGpio() const;
    void    unexportGpio() const;

protected:
    const int           _pinNo;
    const std::string   _path;
    const std::string   _directionFile;
    const std::string   _valueFile;
    const std::string   _activeLowFile;
    const std::string   _edgeFile;
    int                 _pollFd;
    bool                _interruptsSupport;
};

#endif // GPIO_HPP
