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
    explicit Button(const std::string& name);
    ~Button() = default;

    Button(const Button& other) = delete;
    Button& operator=(const Button& other) = delete;

public:
    virtual const std::string&  getName() const override;
    virtual void                serialize(ptree& node) override;
    virtual void                deserialize(const ptree& node) override;

private:
    const std::string   _name;
    int                 _gpioNo;
    GPIO*               _gpio;
};

#endif // BUTTON_HPP
