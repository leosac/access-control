/**
 * \file ledmonitormodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief ledmonitor module class
 */

#ifndef LEDMONITORMODULE_HPP
#define LEDMONITORMODULE_HPP

#include "modules/imonitormodule.hpp"

class Led;

class LedMonitorModule : public IMonitorModule
{
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
    virtual void                blink() override;

private:
    ICore&              _core;
    IHWManager&         _hwmanager;
    const std::string   _name;
    Led*                _led;
    std::string         _ledName;
};

#endif // LEDMONITORMODULE_HPP
