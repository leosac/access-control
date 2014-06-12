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

void ModuleProtocol::pushAuthCommand(AAuthCommand* command)
{
    std::lock_guard<std::mutex> lg(_authCommandsMutex);

    _authCommands.push(command);
}

void ModuleProtocol::cmdCreateAuthRequest(const std::string& source, const std::string& target, const std::string& content)
{
    AuthRequest ar(_authCounter, source, target, content);

    _requests.emplace(_authCounter, ar);
    ++_authCounter;
}

void ModuleProtocol::cmdAuthorize(AuthRequest::Uid id, bool granted)
{
    try {
        AuthRequest&    ar(_requests.at(id));

        static_cast<void>(granted);
        if (granted)
            ar.setState(AuthRequest::Authorized);
        else
            ar.setState(AuthRequest::Denied);
    }
    catch (const std::logic_error& e) {
        logMessage(e.what());
    }
}

void ModuleProtocol::sync()
{
    processCommands();
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

void ModuleProtocol::processCommands()
{
    std::lock_guard<std::mutex> lg(_authCommandsMutex);
    ICommand*                   cmd;

    while (!_authCommands.empty())
    {
        cmd = _authCommands.front();
        _authCommands.pop();
        _authCommandsMutex.unlock();
        cmd->execute();
        delete cmd;
        _authCommandsMutex.lock();
    }
}

void ModuleProtocol::processAuthRequest(AuthRequest& ar)
{
    if ((ar.getDate() + std::chrono::seconds(5)) < system_clock::now())
    {
        ar.setState(AuthRequest::Closed);
        logMessage("AR timed out: uid=" + std::to_string(ar.getId()));
        return ;
    }
    else if (ar.getState() == AuthRequest::New)
    {
        if (!_doorModules.count(ar.getTarget()))
        {
            ar.setState(AuthRequest::Closed);
            logMessage("No such door " + ar.getTarget());
            return ;
        }
        IDoorModule*    door = _doorModules.at(ar.getTarget());

        if (door->isAuthRequired())
        {
            ar.setState(AuthRequest::Waiting);
            _authModule->authenticate(ar);
        }
    }
    else if (ar.getState() == AuthRequest::Waiting)
        return ;
    else if (ar.getState() == AuthRequest::AskAuth)
    {
        ar.setState(AuthRequest::Waiting);
        _authModule->authenticate(ar);
    }
    else if (ar.getState() == AuthRequest::Authorized)
    {
        if (!_doorModules.count(ar.getTarget()))
        {
            ar.setState(AuthRequest::Closed);
            logMessage("No such door " + ar.getTarget());
            return ;
        }
        IDoorModule*    door = _doorModules.at(ar.getTarget());
        door->open();
        ar.setState(AuthRequest::Closed);
    }
    else if (ar.getState() == AuthRequest::Denied)
        ar.setState(AuthRequest::Closed);
}

void ModuleProtocol::registerDoorModule(IModule* module)
{
    IDoorModule*    door;

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
