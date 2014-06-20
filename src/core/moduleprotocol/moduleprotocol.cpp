/**
 * \file moduleprotocol.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief ModuleProtocol class implementation
 */

#include "moduleprotocol.hpp"

#include <chrono>

#include "exception/moduleprotocolexception.hpp"
#include "tools/log.hpp"
#include "authcommands/aauthcommand.hpp"
#include "modules/iauthmodule.hpp"
#include "modules/iloggermodule.hpp"
#include "modules/idoormodule.hpp"
#include "modules/iaccesspointmodule.hpp"
#include "modules/imonitormodule.hpp"

const int ModuleProtocol::AuthRequestValidity = 5;

enum Transition {
    Authorize,
    Deny,
    AskAuth,
    Close
};

ModuleProtocol::ModuleProtocol()
:   _authCounter(0),
    _authModule(nullptr)
{
    _registrationHandler[IModule::ModuleType::Door] = &ModuleProtocol::registerDoorModule;
    _registrationHandler[IModule::ModuleType::AccessPoint] = &ModuleProtocol::registerAccessPointModule;
    _registrationHandler[IModule::ModuleType::Auth] = &ModuleProtocol::registerAuthModule;
    _registrationHandler[IModule::ModuleType::Logger] = &ModuleProtocol::registerLoggerModule;
    _registrationHandler[IModule::ModuleType::ActivityMonitor] = &ModuleProtocol::registerActivityMonitorModule;

    _authLogic.addNode(AuthRequest::New, [this] (AuthRequest& request)
    {
        LOG() << "DFA EXEC: New";
        if (!_doorModules.count(request.getTarget()))
        {
            request.setState(0);
            logMessage("No such door " + request.getTarget());
            return;
        }
        IDoorModule*    door = _doorModules.at(request.getTarget());

        if (door->isAuthRequired())
            _authModule->authenticate(request);
        else
            request.setState(_authLogic.update(request, request.getState(), Authorize));
    } );

    _authLogic.addNode(AuthRequest::AskAuth, [this] (AuthRequest& request)
    {
        LOG() << "DFA EXEC: AskAuth";
        _authModule->authenticate(request);
    } );

    _authLogic.addNode(AuthRequest::Authorized, [this] (AuthRequest& request)
    {
        LOG() << "DFA EXEC: Authorized";
        notifyMonitor(ActivityType::Auth);
        if (!_doorModules.count(request.getTarget()))
        {
            request.setState(0);
            logMessage("No such door " + request.getTarget());
            return;
        }
        IDoorModule*    door = _doorModules.at(request.getTarget());
        door->open();
    } );

    _authLogic.addTransition(AuthRequest::New, Authorize, AuthRequest::Authorized);
    _authLogic.addTransition(AuthRequest::New, Deny, AuthRequest::Denied);
    _authLogic.addTransition(AuthRequest::New, AskAuth, AuthRequest::AskAuth);
    _authLogic.addTransition(AuthRequest::AskAuth, Authorize, AuthRequest::Authorized);
    _authLogic.addTransition(AuthRequest::AskAuth, Deny, AuthRequest::Denied);
}

void ModuleProtocol::logMessage(const std::string& message)
{
    for (auto logger : _loggerModules)
        logger->log(message);
}

void ModuleProtocol::notifyMonitor(ModuleProtocol::ActivityType type)
{
    for (auto monitor : _monitorModules)
        monitor->notify(type);
}

void ModuleProtocol::pushCommand(ICommand::Ptr command)
{
    std::lock_guard<std::mutex> lg(_commandsMutex);

    _commands.push(command);
}

void ModuleProtocol::cmdCreateAuthRequest(const std::string& source, const std::string& target, const std::string& content)
{
    AuthRequest ar(_authCounter, source, target, content);

    _requests.emplace(_authCounter, ar);
    ++_authCounter;

    _authLogic.startNode(_requests.at(ar.getId()), AuthRequest::New);
    LOG() << "CMD: AR created id=" << ar.getId();
}

void ModuleProtocol::cmdAuthorize(AuthRequest::Uid id, bool granted)
{
    if (!_requests.count(id))
    {
        logMessage("Bad auth request id: " + std::to_string(id));
        return;
    }

    AuthRequest&    ar(_requests.at(id));

    ar.setState(_authLogic.update(ar, ar.getState(), granted ? Authorize : Deny));
}

void ModuleProtocol::sync()
{
    processCommands();
    for (auto it = _requests.begin(); it != _requests.end();)
    {
        AuthRequest& ar(it->second);

        if (!ar.getState())
        {
            LOG() << "AR id=" << ar.getId() << " erased";
            it = _requests.erase(it);
        }
        else
        {
            if ((ar.getDate() + std::chrono::seconds(AuthRequestValidity)) < system_clock::now())
            {
                ar.setState(0);
                logMessage("AR timed out: uid=" + std::to_string(ar.getId()));
            }
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

void ModuleProtocol::processCommands()
{
    std::lock_guard<std::mutex> lg(_commandsMutex);
    ICommand::Ptr               cmd;

    while (!_commands.empty())
    {
        cmd = _commands.front();
        _commands.pop();
        _commandsMutex.unlock();
        cmd->execute();
        _commandsMutex.lock();
    }
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
    IMonitorModule* monitor;

    if (!(monitor = dynamic_cast<IMonitorModule*>(module)))
        throw (ModuleProtocolException("Invalid Activity Monitor module"));
    _monitorModules.push_back(monitor);
}
