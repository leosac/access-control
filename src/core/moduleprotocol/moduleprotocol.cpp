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
    Timeout,
    AskAuth,
    Close
};

ModuleProtocol::ModuleProtocol()
:   _authCounter(0),
    _reactivationTime(std::chrono::system_clock::now()),
    _authModule(nullptr)
{
    buildRegistrationHandler();
    buildAuthLogic();
}

void ModuleProtocol::logMessage(const std::string& message, Level level)
{
    for (auto logger : _loggerModules)
        logger->log(message);
    if (level >= Level::Warning)
    {
        for (auto monitor : _monitorModules)
            monitor->notify(ActivityType::System);
    }
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

    if (std::chrono::system_clock::now() < _reactivationTime)
        logMessage("Simultaneous access request discarded");
    else if (!_doorModules.count(ar.getTarget()))
        logMessage("No such door " + ar.getTarget());
    else
    {
        _requests.emplace(_authCounter, ar);
        ++_authCounter;
        _authLogic.startNode(_requests.at(ar.getId()), AuthRequest::New);
        LOG() << "CMD: AR created id=" << ar.getId();
    }
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
            it = _requests.erase(it);
        else
        {
            if ((ar.getTime() + std::chrono::seconds(AuthRequestValidity)) < system_clock::now())
                ar.setState(_authLogic.update(ar, ar.getState(), Timeout));
            ++it;
        }
    }
}

void ModuleProtocol::registerModule(IModule* module)
{
    _registrationHandler.at(module->getType())(module);
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

template <typename T>
static T* castModule(IModule* module)
{
    T* ptr;

    if ((ptr = dynamic_cast<T*>(module)))
        return (ptr);
    else
        throw (ModuleProtocolException("could not cast"));
}

void ModuleProtocol::buildRegistrationHandler()
{
    _registrationHandler.emplace(IModule::ModuleType::Door, [this] (IModule* module)
    {
        IDoorModule* door = castModule<IDoorModule>(module);
        _doorModules.emplace(door->getName(), door);
    } );
    _registrationHandler.emplace(IModule::ModuleType::AccessPoint, [this] (IModule* module)
    {
        IAccessPointModule* ap = castModule<IAccessPointModule>(module);
        _apModules.emplace(ap->getName(), ap);
    } );
    _registrationHandler.emplace(IModule::ModuleType::Auth, [this] (IModule* module)
    {
        if (_authModule != nullptr)
            throw (ModuleProtocolException("Replacing existing Auth module '" + _authModule->getName() +  "' with '" + module->getName() + '\''));
        _authModule = castModule<IAuthModule>(module);
    } );
    _registrationHandler.emplace(IModule::ModuleType::Logger, [this] (IModule* module) { _loggerModules.push_back(castModule<ILoggerModule>(module)); } );
    _registrationHandler.emplace(IModule::ModuleType::ActivityMonitor, [this] (IModule* module) { _monitorModules.push_back(castModule<IMonitorModule>(module)); } );
}

void ModuleProtocol::buildAuthLogic()
{
    _authLogic.addNode(AuthRequest::New, [this] (AuthRequest& request)
    {
        LOG() << "DFA EXEC: New";
        if (_doorModules.at(request.getTarget())->isAuthRequired())
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
        _doorModules.at(request.getTarget())->open();
        _apModules.at(request.getSource())->notifyResponse(true);
    } );

    _authLogic.addNode(AuthRequest::Denied, [this] (AuthRequest& request)
    {
        LOG() << "DFA EXEC: Denied";
        notifyMonitor(ActivityType::Auth);
        _doorModules.at(request.getTarget())->denyAccess();
        _apModules.at(request.getSource())->notifyResponse(false);
    } );

    _authLogic.addTransition(AuthRequest::New, Authorize, AuthRequest::Authorized);
    _authLogic.addTransition(AuthRequest::New, Deny, AuthRequest::Denied);
    _authLogic.addTransition(AuthRequest::New, AskAuth, AuthRequest::AskAuth);
    _authLogic.addTransition(AuthRequest::AskAuth, Authorize, AuthRequest::Authorized);
    _authLogic.addTransition(AuthRequest::AskAuth, Deny, AuthRequest::Denied);
}
