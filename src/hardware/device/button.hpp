/**
 * \file button.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Button class declaration
 */

#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "gpiodevice.hpp"
#include "hardware/iserializabledevice.hpp"

class Button : public ISerializableDevice
{
public:
    explicit Button(const std::string& name, IGPIOProvider& gpioProvider);
    ~Button() = default;

    Button(const Button& other) = delete;
    Button& operator=(const Button& other) = delete;

public:
    virtual const std::string&  getName() const override;
    virtual void                serialize(ptree& node) override;
    virtual void                deserialize(const ptree& node) override;

public:
    bool    isPressed() const;

private:
    const std::string   _name;
    GpioDevice          _gpioDevice;
};

#endif // BUTTON_HPP
