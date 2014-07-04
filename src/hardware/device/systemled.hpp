/**
 * \file systemled.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief SystemLed class declaration
 */

#ifndef SYSTEMLED_HPP
#define SYSTEMLED_HPP

#include <cstdint>

#include "hardware/iserializabledevice.hpp"

class SystemLed : public ISerializableDevice
{
    static const std::string    DevicePathPrefix;

public:
    explicit SystemLed(const std::string& name);
    ~SystemLed() = default;

    SystemLed(const SystemLed& other) = delete;
    SystemLed& operator=(const SystemLed& other) = delete;

public:
    virtual const std::string&  getName() const override;
    virtual void                serialize(ptree& node) override;
    virtual void                deserialize(const ptree& node) override;

public:
    void            blink();

public:
    int             getBrightness() const;
    void            setBrightness(int value) const;
    std::string     getActiveTrigger() const;
    void            setActiveTrigger(const std::string& trigger) const;
    int             getDelayOn() const;
    void            setDelayOn(int milliseconds) const;
    int             getDelayOff() const;
    void            setDelayOff(int milliseconds) const;

private:
    const std::string   _name;
    std::string         _ledName;
    std::string         _path;
    std::string         _brightnessFile;
    std::string         _triggerFile;
    std::string         _delayOnFile;
    std::string         _delayOffFile;
    std::string         _startTrigger;
};

#endif // SYSTEMLED_HPP
