/**
 * \file doormodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief door module class
 */

#include "doormodule.hpp"

#include <sstream>

DoorModule::DoorModule(ICore& core, const std::string& name)
:   _listener(core),
    _name(name)
{}

void DoorModule::notify(const Event& event)
{
    std::stringstream   ss(event.message);
    std::string         uidstr;

    ss >> uidstr;
    _listener.notify(Event(uidstr + " askauth", _name));
}

const std::string& DoorModule::getName() const
{
    return (_name);
}

IModule::ModuleType DoorModule::getType() const
{
    return (ModuleType::Door);
}
