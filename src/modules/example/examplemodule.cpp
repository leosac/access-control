/**
 * \file examplemodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief example module class
 */

#include "examplemodule.hpp"

#include "tools/version.hpp"

ExampleModule::ExampleModule()
:   _version(Version::buildVersionString(0, 1, 0)) {}

ExampleModule::~ExampleModule() {}

ExampleModule::ExampleModule(const ExampleModule& /*other*/) {}

ExampleModule& ExampleModule::operator=(const ExampleModule& /*other*/)
{
    return (*this);
}

void ExampleModule::sendEvent(const Event& event)
{
    static_cast<void>(event);
}

IModule::Type ExampleModule::getType() const
{
    return (Logger);
}

const std::string& ExampleModule::getVersionString() const
{
    return (_version);
}
