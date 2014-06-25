/**
 * \file buzzer.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Buzzer class declaration
 */

#ifndef BUZZER_HPP
#define BUZZER_HPP

#include "agpiodevice.hpp"

class Buzzer : public AGpioDevice
{
public:
    explicit Buzzer(const std::string& name, IGPIOProvider& gpioProvider);
    ~Buzzer() = default;

    Buzzer(const Buzzer& other) = delete;
    Buzzer& operator=(const Buzzer& other) = delete;

public:
    virtual void    serialize(ptree& node) override;
    virtual void    deserialize(const ptree& node) override;

public:
    /**
     *  FIXME uses blocking logic
     */
    void    beep(unsigned int frequency, unsigned int durationMs);
};

#endif // BUZZER_HPP
