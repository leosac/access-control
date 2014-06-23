/**
 * \file buzzer.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Buzzer class declaration
 */

#ifndef BUZZER_HPP
#define BUZZER_HPP

#include "hardware/iserializabledevice.hpp"
#include "gpio/igpioprovider.hpp"

class Buzzer : public ISerializableDevice
{
public:
    explicit Buzzer(const std::string& name, IGPIOProvider& gpioObservable);
    ~Buzzer() = default;

    Buzzer(const Buzzer& other) = delete;
    Buzzer& operator=(const Buzzer& other) = delete;

public:
    virtual const std::string&  getName() const override;
    virtual void                serialize(ptree& node) override;
    virtual void                deserialize(const ptree& node) override;

public:
    void    beep(unsigned int frequency, unsigned int durationMs);

private:
    const std::string   _name;
    IGPIOProvider&      _gpioProvider;
    int                 _gpioNo;
    GPIO*               _gpio;
};

#endif // BUZZER_HPP
