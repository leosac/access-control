/**
 * \file wiegandmodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief wiegand module class
 */

#ifndef WIEGANDMODULE_HPP
#define WIEGANDMODULE_HPP

#include "modules/imodule.hpp"
#include "hardware/device/wiegandinterface.hpp"
#include "hardware/device/iwiegandlistener.hpp"

class WiegandModule : public IModule, public IWiegandListener
{
public:
    explicit WiegandModule(ICore& core);
    ~WiegandModule();

    WiegandModule(const WiegandModule& other) = delete;
    WiegandModule& operator=(const WiegandModule& other) = delete;

public:
    void                notifyCardRead(const CardId& cardId);
    void                notify(const Event& event);
    Type                getType() const;
    const std::string&  getVersionString() const;

private:
    IEventListener&     _listener;
    const std::string   _version;
    WiegandInterface*   _interface;
};

#endif // WIEGANDMODULE_HPP
