/**
 * \file hwmanager.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief hardware managing class
 */

#ifndef HWMANAGER_HPP
#define HWMANAGER_HPP

#include <hardware/device/gpio/pfdigital.hpp>
#include "ihwmanager.hpp"
#include "config/xmlconfig.hpp"
#include "config/ixmlserializable.hpp"

#include "iserializabledevice.hpp"
#include "device/gpio/gpiomanager.hpp"
#include "device/iwiegandlistener.hpp"

class DIPSwitch;

class HWManager : public IHWManager, public IXmlSerializable
{
    struct Device {
        std::string             type;
        ISerializableDevice*    instance;
    };

public:
    using StateHook = std::function<void(bool)>;
    enum class HookType {
        DHCP = 0,
        DefaultIp,
        Reset
    };

public:
    explicit HWManager() = default;
    ~HWManager() = default;

    HWManager(const HWManager& other) = delete;
    HWManager& operator=(const HWManager& other) = delete;

public:
    virtual void serialize(ptree& node) override;
    virtual void deserialize(const ptree& node) override;

public:
    virtual void                start() override;
    virtual void                stop() override;
    virtual IDevice*            getDevice(const std::string& name) override;
    virtual const PlatformInfo& getPlatformInfo() const override;

public:
    void    setStateHook(HookType type, StateHook hook);
    void    sync();

private:
    ISerializableDevice*    buildDevice(const std::string& type, const std::string& name);

private:
    PFDigital                       pfdigital_gpio_manager_;
    GPIOManager                     _gpioManager;
    PlatformInfo                    _platform;
    std::map<std::string, Device>   _devices;
    std::map<HookType, StateHook>   _hooks;
    DIPSwitch*                      _masterDipSwitch;
};

#endif // HWMANAGER_HPP
