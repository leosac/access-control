/**
 * \file wiegandmodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief wiegand module class
 */

#include "wiegandmodule.hpp"

#include "tools/version.hpp"

WiegandModule::WiegandModule(ICore& core)
:   _listener(core),
    _version(Version::buildVersionString(0, 1, 0)),
    _interface(core.getHWManager()->buildWiegandInterface(this))
{}

WiegandModule::~WiegandModule()
{
    delete _interface;
}

void WiegandModule::notifyCardRead(const IWiegandListener::CardId& cardId)
{
    _listener.notify(Event("MESSG", "wiegandmodule", "rpleth.so-debug"));
}

void WiegandModule::notify(const Event& event)
{
    static_cast<void>(event);
}

IModule::Type WiegandModule::getType() const
{
    return (AccessPoint);
}

const std::string& WiegandModule::getVersionString() const
{
    return (_version);
}
