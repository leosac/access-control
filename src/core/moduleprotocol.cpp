/**
 * \file moduleprotocol.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief ModuleProtocol class implementation
 */

#include "moduleprotocol.hpp"

#include <chrono>

#include "exception/moduleprotocolexception.hpp"
#include "tools/log.hpp"
#include "modules/iauthmodule.hpp"
#include "modules/iloggermodule.hpp"
#include "modules/idoormodule.hpp"
#include "modules/iaccesspointmodule.hpp"

ModuleProtocol::ModuleProtocol()
:   _authCounter(0),
    _authModule(nullptr)
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

void ModuleProtocol::createAuthRequest(const std::string& source, const std::string& target, const std::string& content)
{
    AuthRequest ar(_authCounter, source, target, content);

    _requests.emplace(_authCounter, ar);
    ++_authCounter;
}

void ModuleProtocol::authorize(AuthRequest::Uid id, bool granted)
{
    try {
        AuthRequest&    ar(_requests.at(id));

        static_cast<void>(granted);
        ar.setState(AuthRequest::Closed);
    }
    catch (const std::logic_error& e) {
        logMessage(e.what());
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

void ModuleProtocol::printDebug()
{
    LOG() << "Registered modules:";
    LOG() << "AuthModule " << _authModule->getName();
    for (auto a : _doorModules)
        LOG() << "Door " << a.second->getName();
    for (auto a : _apModules)
        LOG() << "AccessPoint " << a.second->getName();
    for (auto a : _loggerModules)
        LOG() << "Logger " << a->getName();
}

void ModuleProtocol::processAuthRequest(AuthRequest& ar)
{
    if ((ar.getDate() + std::chrono::seconds(5)) < system_clock::now())
    {
        ar.setState(AuthRequest::Closed);
        logMessage("AR timed out: uid=" + std::to_string(ar.getId()));
        return ;
    }

    return ;// FIXME

    if (!_doorModules.count(ar.getTarget()))
    {
        ar.setState(AuthRequest::Closed);
        logMessage("No such door");
        return ;
    }
    IDoorModule*    door = _doorModules.at(ar.getTarget());

    if (!door->isAuthRequired())
    {
        door->open();
        logMessage("No auth required, opening");
        ar.setState(AuthRequest::Closed);
        return ;
    }

    if (_authModule->authenticate(ar))
    {
        logMessage("Access granted");
        door->open();
    }
    else
        logMessage("Access denied");

    ar.setState(AuthRequest::Closed);
}

void ModuleProtocol::registerDoorModule(IModule* module)
{
    IDoorModule* door;

    if (!(door = dynamic_cast<IDoorModule*>(module)))
        throw (ModuleProtocolException("Invalid Door module"));
    _doorModules.emplace(door->getName(), door);
}

void ModuleProtocol::registerAccessPointModule(IModule* module)
{
    IAccessPointModule* ap;

    if (!(ap = dynamic_cast<IAccessPointModule*>(module)))
        throw (ModuleProtocolException("Invalid AccessPoint module"));
    _apModules.emplace(ap->getName(), ap);
}

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

void ModuleProtocol::registerActivityMonitorModule(IModule* module)
{
    static_cast<void>(module);
}
