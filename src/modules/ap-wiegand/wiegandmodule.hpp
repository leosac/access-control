/**
 * \file wiegandmodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief wiegand module class
 */

#ifndef WIEGANDMODULE_HPP
#define WIEGANDMODULE_HPP

#include <list>
#include <mutex>

#include "modules/iaccesspointmodule.hpp"
#include "modules/iaccessrequester.hpp"
#include "hardware/ihwmanager.hpp"
#include "wiegandrequester.hpp"

class WiegandModule : public IAccessPointModule
{
    using RequesterList = std::list<IAccessRequester*>;

public:
    explicit WiegandModule(ICore& core, const std::string& name);
    ~WiegandModule() = default;

    WiegandModule(const WiegandModule& other) = delete;
    WiegandModule& operator=(const WiegandModule& other) = delete;

public:
    virtual const std::string&  getName() const override;
    virtual ModuleType          getType() const override;
    virtual void                serialize(ptree& node) override;
    virtual void                deserialize(const ptree& node) override;

    /**
    * Called by WiegandRequested to notify a access was attempted.
    * This will build a AuthCommand and send it.
    */
    virtual void                notifyAccess(const std::string& request) override;
    virtual void                notifyResponse(bool granted) override;

private:
    ICore&              _core;
    const std::string   _name;
    unsigned int        _hiGPIO;
    unsigned int        _loGPIO;
    std::string         _target;
    IHWManager&         _hwmanager;
    std::string         _deviceName;
    std::mutex          _notifyMutex;
    RequesterList       _requesterList;
};

#endif // WIEGANDMODULE_HPP
