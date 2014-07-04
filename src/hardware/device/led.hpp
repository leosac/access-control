/**
 * \file led.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Led class declaration
 * NOTE not thread-safe on multiple long turn-ons.
 */

#ifndef LED_HPP
#define LED_HPP

#include <atomic>

#include "hardware/iserializabledevice.hpp"
#include "gpiodevice.hpp"

class Led : public ISerializableDevice
{
public:
    explicit Led(const std::string& name, IGPIOProvider& gpioProvider);
    ~Led() = default;

    Led(const Led& other) = delete;
    Led& operator=(const Led& other) = delete;

public:
    virtual const std::string&  getName() const override;
    virtual void                serialize(ptree& node) override;
    virtual void                deserialize(const ptree& node) override;

public:
    void    turnOn();
    void    turnOn(unsigned int durationMs);
    void    turnOff();
    void    toggle();

private:
    const std::string   _name;
    GpioDevice          _gpioDevice;
    std::atomic<int>    _count;
};

#endif // LED_HPP
