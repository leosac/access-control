/**
 * \file wiegandmodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief wiegand module class
 */

#include "wiegandmodule.hpp"

#include "tools/version.hpp"

WiegandModule::WiegandModule(IEventListener* listener)
:   _listener(listener),
    _version(Version::buildVersionString(0, 1, 0))
{}

WiegandModule::~WiegandModule() {}

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
