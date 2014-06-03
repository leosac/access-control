/**
 * \file button.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Button class declaration
 */

#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "hardware/iserializabledevice.hpp"

class GPIO;
class Button : public ISerializableDevice
{
public:
    explicit Button() = default;
    ~Button() = default;

    Button(const Button& other) = delete;
    Button& operator=(const Button& other) = delete;

public:
    virtual IDevice::DeviceType getType() const override;
    virtual void                serialize(boost::property_tree::ptree& node) override;
    virtual void                deserialize(const boost::property_tree::ptree& node) override;

private:
    int     _gpioNo;
    GPIO*   _gpio;
};

#endif // BUTTON_HPP
