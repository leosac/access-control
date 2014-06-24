/**
 * \file relay.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Relay class declaration
 */

#ifndef RELAY_HPP
#define RELAY_HPP

#include "hardware/iserializabledevice.hpp"
#include "gpio/igpioprovider.hpp"

class Relay : public ISerializableDevice
{
public:
    explicit Relay(const std::string& name, IGPIOProvider& gpioObservable);
    ~Relay() = default;

    Relay(const Relay& other) = delete;
    Relay& operator=(const Relay& other) = delete;

public:
    virtual const std::string&  getName() const override;
    virtual void                serialize(ptree& node) override;
    virtual void                deserialize(const ptree& node) override;

public:
    void    open();
    void    close();
    void    setOpen(bool state);

private:
    const std::string   _name;
    IGPIOProvider&      _gpioProvider;
    int                 _gpioNo;
    GPIO*               _gpio;
};

#endif // RELAY_HPP
