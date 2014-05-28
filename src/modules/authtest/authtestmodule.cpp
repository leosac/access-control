/**
 * \file authtestmodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief simple auth module
 */

#include "authtestmodule.hpp"

#include <sstream>

AuthTestModule::AuthTestModule(IEventListener& listener, const std::string& name)
:   _listener(listener),
    _name(name)
{}

void AuthTestModule::notify(const Event& event)
{
    std::istringstream  iss(event.message);
    CardId              cid;
    unsigned int        val;
    std::string         uidstr;
    std::string         opcode;

    iss >> uidstr;
    iss >> opcode;

    while (iss >> val)
        cid.push_back(static_cast<Byte>(val));

    if (_auth.hasAccess(cid))
        _listener.notify(Event(uidstr + " granted", _name));
    else
        _listener.notify(Event(uidstr + " denied", _name));
}

const std::string& AuthTestModule::getName() const
{
    return (_name);
}

IModule::ModuleType AuthTestModule::getType() const
{
    return (ModuleType::Auth);
}

void AuthTestModule::serialize(boost::property_tree::ptree& node)
{
    boost::property_tree::ptree& child = node.add("properties", std::string());

    _auth.serialize(child);
}

void AuthTestModule::deserialize(const boost::property_tree::ptree& node)
{
    _auth.deserialize(node.get_child("properties").get_child("auth"));
}
