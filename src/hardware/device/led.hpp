/**
 * \file led.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Led class declaration
 */

#ifndef LED_HPP
#define LED_HPP

#include "agpiodevice.hpp"

class Led : public AGpioDevice
{
public:
    explicit Led(const std::string& name, IGPIOProvider& gpioProvider);
    ~Led() = default;

    Led(const Led& other) = delete;
    Led& operator=(const Led& other) = delete;

public:
    virtual void    serialize(ptree& node) override;
    virtual void    deserialize(const ptree& node) override;

public:
    void    blink();
};

#endif // LED_HPP
