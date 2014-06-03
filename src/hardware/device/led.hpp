/**
 * \file led.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Led class declaration
 */

#ifndef LED_HPP
#define LED_HPP

#include "hardware/iserializabledevice.hpp"
#include "gpio/igpioprovider.hpp"

class GPIO;

class Led : public ISerializableDevice
{
public:
    explicit Led(IGPIOProvider& gpioProvider);
    ~Led() = default;

    Led(const Led& other) = delete;
    Led& operator=(const Led& other) = delete;

public:
    virtual void serialize(boost::property_tree::ptree& node) override;
    virtual void deserialize(const boost::property_tree::ptree& node) override;
    virtual DeviceType getType() const override;

public:
    void    blink();

private:
    IGPIOProvider&  _gpioProvider;
    int             _gpioNo;
    GPIO*           _gpio;
};

#endif // LED_HPP
