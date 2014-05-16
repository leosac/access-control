/**
 * \file doormodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief door module class
 */

#include "doormodule.hpp"

#include <sstream>

#include "tools/version.hpp"

DoorModule::DoorModule(ICore& core, const std::string& name)
:   _listener(core),
    _name(name),
    _version(Version::buildVersionString(0, 1, 0))
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

const std::string& DoorModule::getVersionString() const
{
    return (_version);
}
