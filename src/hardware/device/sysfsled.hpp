/**
 * \file sysfsled.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief SysFsLed class declaration
 */

#ifndef SYSFSLED_HPP
#define SYSFSLED_HPP

#include <cstdint>

#include "hardware/iserializabledevice.hpp"

class SysFsLed : public ISerializableDevice
{
    static const std::string    DevicePathPrefix;

public:
    explicit SysFsLed(const std::string& name);
    ~SysFsLed() = default;

    SysFsLed(const SysFsLed& other) = delete;
    SysFsLed& operator=(const SysFsLed& other) = delete;

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

#endif // SYSFSLED_HPP
