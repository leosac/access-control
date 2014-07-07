/**
 * \file button.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Button class declaration
 */

#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <functional>

#include "gpiodevice.hpp"
#include "hardware/iserializabledevice.hpp"

class Button : public ISerializableDevice, public IGPIOListener
{
    typedef std::function<void()> Callback;

public:
    explicit Button(const std::string& name, IGPIOProvider& gpioProvider);
    ~Button() = default;

    Button(const Button& other) = delete;
    Button& operator=(const Button& other) = delete;

public:
    virtual const std::string&  getName() const override;
    virtual void                serialize(ptree& node) override;
    virtual void                deserialize(const ptree& node) override;
    virtual void                notify(int gpioNo) override;
    virtual void                timeout() override;


public:
    bool    isPressed() const;
    void    setCallback(std::function<void()> callback);

private:
    const std::string   _name;
    GpioDevice          _gpioDevice;
    Callback            _callback;
};

#endif // BUTTON_HPP
