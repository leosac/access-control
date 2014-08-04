/**
 * \file buzzer.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Buzzer class declaration
 */

#ifndef BUZZER_HPP
#define BUZZER_HPP

#include "gpiodevice.hpp"
#include "hardware/iserializabledevice.hpp"

class Buzzer : public ISerializableDevice
{
public:
    explicit Buzzer(const std::string& name, IGPIOProvider& gpioProvider);
    ~Buzzer() = default;

    Buzzer(const Buzzer& other) = delete;
    Buzzer& operator=(const Buzzer& other) = delete;

public:
    virtual const std::string&  getName() const override;
    virtual void                serialize(ptree& node) override;
    virtual void                deserialize(const ptree& node) override;

public:
    void    beep(unsigned int durationMs, float frequencyHz);
    void    beep(unsigned int durationMs); // NOTE Use buzzer default frequency

private:
    const std::string   _name;
    float               _frequency;
    GpioDevice          _gpioDevice;
};

#endif // BUZZER_HPP
