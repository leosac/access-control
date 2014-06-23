/**
 * \file authtestmodule.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief simple auth module
 */

#include "authtestmodule.hpp"
#include "core/moduleprotocol/authcommands/authcmdgrantaccess.hpp"
#include "core/moduleprotocol/authcommands/authcmddenyaccess.hpp"
#include "config/xmlconfig.hpp"
#include "tools/log.hpp"

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
    XmlConfig   conf(_configPath, _auth);
    conf.serialize();

    node.put<std::string>("configpath", _configPath);
    _auth.serialize(node);
}

void AuthTestModule::deserialize(const ptree& node)
{
    _configPath = node.get<std::string>("configpath");

    XmlConfig   conf(_configPath, _auth);
    conf.deserialize();
}

void AuthTestModule::authenticate(const AuthRequest& ar)
{
    std::istringstream          iss(ar.getContent());
    CardId                      cid;
    unsigned int                byte;

    LOG() << "AR content=" << ar.getContent();

    while (iss >> byte)
        cid.push_back(static_cast<Byte>(byte));

    if (_auth.hasAccess(cid))
        _protocol.pushCommand(ICommand::Ptr(new AuthCmdGrantAccess(&_protocol, ar.getId())));
    else
        _protocol.pushCommand(ICommand::Ptr(new AuthCmdDenyAccess(&_protocol, ar.getId())));
}
