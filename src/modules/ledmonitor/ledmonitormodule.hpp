/**
 * \file ledmonitormodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief ledmonitor module class
 */

#ifndef LEDMONITORMODULE_HPP
#define LEDMONITORMODULE_HPP

#include <map>

#include "modules/imonitormodule.hpp"

class SysFsLed;

class LedMonitorModule : public IMonitorModule
{
    static const std::string TypeStrings[IModuleProtocol::ActivityTypes];
    struct MonitorLed {
        std::string deviceName;
        SysFsLed*  instance;
    };

public:
    explicit LedMonitorModule(ICore& core, const std::string& name);
    ~LedMonitorModule() = default;

    LedMonitorModule(const LedMonitorModule& other) = delete;
    LedMonitorModule& operator=(const LedMonitorModule& other) = delete;

public:
    virtual const std::string&  getName() const override;
    virtual ModuleType          getType() const override;
    virtual void                serialize(ptree& node) override;
    virtual void                deserialize(const ptree& node) override;
    virtual void                notify(IModuleProtocol::ActivityType type) override;

private:
    ICore&                                              _core;
    IHWManager&                                         _hwmanager;
    const std::string                                   _name;
    std::map<IModuleProtocol::ActivityType, MonitorLed> _leds;
};

#endif // LEDMONITORMODULE_HPP
