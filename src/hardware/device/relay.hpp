/**
 * \file relay.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Relay class declaration
 */

#ifndef RELAY_HPP
#define RELAY_HPP

#include "agpiodevice.hpp"

class Relay : public AGpioDevice
{
public:
    explicit Relay(const std::string& name, IGPIOProvider& gpioObservable);
    ~Relay() = default;

    Relay(const Relay& other) = delete;
    Relay& operator=(const Relay& other) = delete;

public:
    virtual void    serialize(ptree& node) override;
    virtual void    deserialize(const ptree& node) override;

public:
    void    open();
    void    close();
    void    setOpen(bool state);
};

#endif // RELAY_HPP
