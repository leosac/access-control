/**
 * \file authtestmodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief simple auth module
 */

#include "authtestmodule.hpp"
#include "core/moduleprotocol/authcommands/authcmdgrantaccess.hpp"
#include "core/moduleprotocol/authcommands/authcmddenyaccess.hpp"

#include <sstream>

AuthTestModule::AuthTestModule(ICore& core, const std::string& name)
:   _protocol(core.getModuleProtocol()),
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

void AuthTestModule::authenticate(const AuthRequest& ar)
{
    std::istringstream          iss(ar.getContent());
    CardId                      cid;
    unsigned int                byte;

    while (iss >> byte)
        cid.push_back(static_cast<Byte>(byte));

    if (_auth.hasAccess(cid))
        _protocol.pushAuthCommand(new AuthCmdGrantAccess(&_protocol, ar.getId()));
    else
        _protocol.pushAuthCommand(new AuthCmdDenyAccess(&_protocol, ar.getId()));
}
