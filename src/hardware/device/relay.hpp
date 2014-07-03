/**
 * \file relay.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Relay class declaration
 */

#ifndef RELAY_HPP
#define RELAY_HPP

#include <thread>
#include <atomic>
#include <mutex>

#include "gpiodevice.hpp"
#include "hardware/iserializabledevice.hpp"

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
    void    open(unsigned int durationMs);
    void    close();
    void    setOpen(bool state);
    bool    isOpen();

private:
    const std::string   _name;
    GpioDevice          _gpioDevice;
    std::mutex          _openMutex;
    std::atomic<bool>   _isOpen;
};

#endif // RELAY_HPP
