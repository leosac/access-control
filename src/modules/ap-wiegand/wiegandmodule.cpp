/**
 * \file wiegandmodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief wiegand module class
 */

#include "wiegandmodule.hpp"

#include <sstream>

#include "tools/version.hpp"

WiegandModule::WiegandModule(ICore& core)
:   _listener(core),
    _version(Version::buildVersionString(0, 1, 0)),
    _interface(core.getHWManager()->buildWiegandInterface(this, 15, 14))
{}

WiegandModule::~WiegandModule()
{
    delete _interface;
}

void WiegandModule::notifyCardRead(const IWiegandListener::CardId& cardId)
{
    std::stringstream  ss;

    for (std::size_t i = 0; i < cardId.size(); ++i)
    {
        if (i > 0)
            ss << ' ';
        ss << static_cast<unsigned int>(cardId[i]);
    }
    _listener.notify(Event(ss.str(), "wiegandmodule", "librpleth.so-debug")); // FIXME Debug
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
