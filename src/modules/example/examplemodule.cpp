/**
 * \file examplemodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief example module class
 */

#include "examplemodule.hpp"

ExampleModule::ExampleModule(const std::string& name)
:   _name(name)
{}

void ExampleModule::notify(const Event& event)
{
    static_cast<void>(event);
}

const std::string& ExampleModule::getName() const
{
    return (_name);
}

IModule::ModuleType ExampleModule::getType() const
{
    return (ModuleType::Logger);
}
