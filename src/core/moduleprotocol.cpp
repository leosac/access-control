/**
 * \file moduleprotocol.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief ModuleProtocol class implementation
 */

#include "moduleprotocol.hpp"

#include <chrono>

#include "exception/moduleprotocolexception.hpp"

#include "modules/iauthmodule.hpp"
#include "modules/iloggermodule.hpp"

ModuleProtocol::ModuleProtocol()
:   _authCounter(0)
{
    _registrationHandler[IModule::ModuleType::Door] = &ModuleProtocol::registerDoorModule;
    _registrationHandler[IModule::ModuleType::AccessPoint] = &ModuleProtocol::registerAccessPointModule;
    _registrationHandler[IModule::ModuleType::Auth] = &ModuleProtocol::registerAuthModule;
    _registrationHandler[IModule::ModuleType::Logger] = &ModuleProtocol::registerLoggerModule;
    _registrationHandler[IModule::ModuleType::ActivityMonitor] = &ModuleProtocol::registerActivityMonitorModule;
}

void ModuleProtocol::logMessage(const std::string& message)
{
    for (auto logger : _loggerModules)
        logger->log(message);
}

void ModuleProtocol::createAuthRequest(const std::string& content, const std::string& target)
{
    AuthRequest ar(_authCounter, content, target);

    _requests.emplace(_authCounter, ar);
    ++_authCounter;
}

void ModuleProtocol::authorize(AuthRequest::Uid id, bool granted)
{
    try {
        AuthRequest&    ar(_requests.at(id));

        // TODO
        static_cast<void>(granted);
        ar.setState(AuthRequest::Closed);
    }
    catch (const std::logic_error& e) {
        // TODO
    }
}

void ModuleProtocol::sync()
{
    for (auto it = _requests.begin(); it != _requests.end();)
    {
        AuthRequest& ar(it->second);

        if (ar.getState() == AuthRequest::Closed)
            it = _requests.erase(it);
        else
        {
            processAuthRequest(ar);
            ++it;
        }
    }
}

void ModuleProtocol::registerModule(IModule* module)
{
    RegisterFunc    func = _registrationHandler[module->getType()];

    if (!func)
        throw (ModuleProtocolException("Unknown module type"));
    ((*this).*func)(module);
}

void ModuleProtocol::processAuthRequest(AuthRequest& ar)
{
    if (ar.getDate() + std::chrono::seconds(5) < system_clock::now())
    {
        ar.setState(AuthRequest::Closed);
        logMessage("AR timed out: uid=" + std::to_string(ar.getId()));
    }
}

void ModuleProtocol::registerDoorModule(IModule* /*module*/) {}

void ModuleProtocol::registerAccessPointModule(IModule* /*module*/) {}

void ModuleProtocol::registerAuthModule(IModule* module)
{
    IAuthModule*    auth;

    if (!(auth = dynamic_cast<IAuthModule*>(module)))
        throw (ModuleProtocolException("Invalid Auth module"));
    if (_authModule)
        throw (ModuleProtocolException("Replacing existing Auth module"));
    _authModule = auth;
}

void ModuleProtocol::registerLoggerModule(IModule* module)
{
    ILoggerModule* logger;

    if (!(logger = dynamic_cast<ILoggerModule*>(module)))
        throw (ModuleProtocolException("Invalid Logger module"));
    _loggerModules.push_back(logger);
}

void ModuleProtocol::registerActivityMonitorModule(IModule* /*module*/) {}
