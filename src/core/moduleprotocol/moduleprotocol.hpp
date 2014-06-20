/**
 * \file moduleprotocol.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief ModuleProtocol class declaration
 */

#ifndef MODULEPROTOCOL_HPP
#define MODULEPROTOCOL_HPP

#include <map>
#include <string>
#include <list>
#include <mutex>
#include <queue>

#include "modules/imodule.hpp"
#include "imoduleprotocol.hpp"
#include "tools/dfautomata.hpp"

class IAccessPointModule;
class IDoorModule;
class IAuthModule;
class IMonitorModule;
class ILoggerModule;

class ModuleProtocol : public IModuleProtocol
{
    static const int AuthRequestValidity;
    typedef void (ModuleProtocol::*RegisterFunc)(IModule*);

public:
    explicit ModuleProtocol();
    ~ModuleProtocol() = default;

    ModuleProtocol(const ModuleProtocol& other) = delete;
    ModuleProtocol& operator=(const ModuleProtocol& other) = delete;

public:
    virtual void    logMessage(const std::string& message) override;
    virtual void    notifyMonitor(IModuleProtocol::ActivityType type) override;
    virtual void    pushCommand(ICommand::Ptr command) override;

public:
    virtual void    cmdCreateAuthRequest(const std::string& source, const std::string& target, const std::string& content) override;
    virtual void    cmdAuthorize(AuthRequest::Uid id, bool granted) override;

public:
    void    sync();
    void    registerModule(IModule* module);

private:
    void    processCommands();

private:
    void    registerDoorModule(IModule* module);
    void    registerAccessPointModule(IModule* module);
    void    registerAuthModule(IModule* module);
    void    registerLoggerModule(IModule* module);
    void    registerActivityMonitorModule(IModule* module);

private:
    AuthRequest::Uid                            _authCounter;
    std::map<AuthRequest::Uid, AuthRequest>     _requests;
    std::mutex                                  _commandsMutex;
    std::queue<ICommand::Ptr>                   _commands;
    DFAutomata<AuthRequest, int, int>           _authLogic;

private:
    std::map<IModule::ModuleType, RegisterFunc> _registrationHandler;
    std::list<ILoggerModule*>                   _loggerModules;
    std::list<IMonitorModule*>                  _monitorModules;
    std::map<std::string, IDoorModule*>         _doorModules;
    std::map<std::string, IAccessPointModule*>  _apModules;
    IAuthModule*                                _authModule;
};

#endif // MODULEPROTOCOL_HPP
