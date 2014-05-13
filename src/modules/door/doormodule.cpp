/**
 * \file doormodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief door module class
 */

#include "doormodule.hpp"

#include "tools/version.hpp"

DoorModule::DoorModule()
:   _version(Version::buildVersionString(0, 1, 0))
{}

void DoorModule::notify(const Event& event)
{
    static_cast<void>(event);

    
}

IModule::ModuleType DoorModule::getType() const
{
    return (ModuleType::Door);
}

const std::string& DoorModule::getVersionString() const
{
    return (_version);
}
