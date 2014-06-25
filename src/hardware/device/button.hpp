/**
 * \file button.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Button class declaration
 */

#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "agpiodevice.hpp"

class Button : public AGpioDevice
{
public:
    explicit Button(const std::string& name, IGPIOProvider& gpioProvider);
    ~Button() = default;

    Button(const Button& other) = delete;
    Button& operator=(const Button& other) = delete;

public:
    virtual void    deserialize(const ptree& node) override;

public:
    bool    isPressed() const;
};

#endif // BUTTON_HPP
