/**
 * \file wiegandmodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief wiegand module class
 */

#include "wiegandmodule.hpp"

#include <sstream>

WiegandModule::WiegandModule(ICore& core, const std::string& name)
:   _listener(core),
    _name(name),
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
    _listener.notify(Event(ss.str(), _name, "libdoor.so-default"));
}

void WiegandModule::notify(const Event& event)
{
    static_cast<void>(event);
}

const std::string& WiegandModule::getName() const
{
    return (_name);
}

IModule::ModuleType WiegandModule::getType() const
{
    return (ModuleType::AccessPoint);
}
