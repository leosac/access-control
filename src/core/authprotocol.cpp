/**
 * \file authprotocol.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief AuthProtocol class implementation
 */

#include "authprotocol.hpp"

#include "exception/authprotocolexception.hpp"

#include "modules/iauthmodule.hpp"
#include "modules/iloggermodule.hpp"

AuthProtocol::AuthProtocol()
:   _authCounter(0)
{
    _registrationHandler[IModule::ModuleType::Door] = &AuthProtocol::registerDoorModule;
    _registrationHandler[IModule::ModuleType::AccessPoint] = &AuthProtocol::registerAccessPointModule;
    _registrationHandler[IModule::ModuleType::Auth] = &AuthProtocol::registerAuthModule;
    _registrationHandler[IModule::ModuleType::Logger] = &AuthProtocol::registerLoggerModule;
    _registrationHandler[IModule::ModuleType::ActivityMonitor] = &AuthProtocol::registerActivityMonitorModule;
}

void AuthProtocol::createAuthRequest(const std::string& content)
{
    AuthRequest ar(_authCounter, content);

    _requestList.emplace(_authCounter, ar);
    ++_authCounter;
}

void AuthProtocol::sync()
{
    for (auto it = _requestList.begin(); it != _requestList.end();)
    {
        AuthRequest& ar(it->second);

        if (ar.getState() == AuthRequest::Closed)
            it = _requestList.erase(it);
        else
        {
            // FIXME
            ++it;
        }
    }
}

//     if (!_authModule)
//         throw (CoreException("No auth module loaded"));

void AuthProtocol::registerModule(IModule* module)
{
    RegisterFunc    func = _registrationHandler[module->getType()];

    if (!func)
        throw (AuthProtocolException("Unknown module type"));
    ((*this).*func)(module);
}

void AuthProtocol::registerDoorModule(IModule* /*module*/) {}

void AuthProtocol::registerAccessPointModule(IModule* /*module*/) {}

void AuthProtocol::registerAuthModule(IModule* module)
{
    IAuthModule*    auth;

    if (!(auth = dynamic_cast<IAuthModule*>(module)))
        throw (AuthProtocolException("Invalid Auth module"));
    if (_authModule)
        throw (AuthProtocolException("Replacing existing Auth module"));
    _authModule = auth;
}

void AuthProtocol::registerLoggerModule(IModule* module)
{
    ILoggerModule* logger;

    if (!(logger = dynamic_cast<ILoggerModule*>(module)))
        throw (AuthProtocolException("Invalid Logger module"));
    _loggerModules.push_back(logger);
}

void AuthProtocol::registerActivityMonitorModule(IModule* /*module*/) {}
