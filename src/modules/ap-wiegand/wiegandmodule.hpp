/**
 * \file wiegandmodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief wiegand module class
 */

#ifndef WIEGANDMODULE_HPP
#define WIEGANDMODULE_HPP

#include "modules/imodule.hpp"
#include "hardware/device/iwiegandlistener.hpp"
#include "hardware/ihwmanager.hpp"

class Led;
class WiegandReader;

class WiegandModule : public IModule, public IWiegandListener
{
public:
    explicit WiegandModule(ICore& core, const std::string& name);
    ~WiegandModule() = default;

    WiegandModule(const WiegandModule& other) = delete;
    WiegandModule& operator=(const WiegandModule& other) = delete;

public:
    virtual void                notifyCardRead(const CardId& cardId) override;
    virtual const std::string&  getName() const override;
    virtual ModuleType          getType() const override;
    virtual void                serialize(ptree& node) override;
    virtual void                deserialize(const ptree& node) override;

private:
    ICore&              _core;
    const std::string   _name;
    unsigned int        _hiGPIO;
    unsigned int        _loGPIO;
    std::string         _target;
    IHWManager&         _hwmanager;
    WiegandReader*      _interface;
    std::string         _interfaceName;
};

#endif // WIEGANDMODULE_HPP
