/**
 * \file authtestmodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief simple auth module
 */

#include "authtestmodule.hpp"

#include <sstream>

AuthTestModule::AuthTestModule(ICore& core, const std::string& name)
:   _core(core),
    _name(name)
{}

const std::string& AuthTestModule::getName() const
{
    return (_name);
}

IModule::ModuleType AuthTestModule::getType() const
{
    return (ModuleType::Auth);
}

void AuthTestModule::serialize(ptree& node)
{
    ptree& child = node.add("properties", std::string());

    _auth.serialize(child);
}

void AuthTestModule::deserialize(const ptree& node)
{
    _auth.deserialize(node.get_child("properties").get_child("auth"));
}
