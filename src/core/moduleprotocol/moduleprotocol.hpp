/**
 * \file moduleprotocol.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief ModuleProtocol class declaration
 * NOTE before unloading modules, make sure there's no access request still
 * processing, or std::map out_of_range exceptions will start to appear.
 */

#ifndef MODULEPROTOCOL_HPP
#define MODULEPROTOCOL_HPP

#include <map>
#include <string>
#include <list>
#include <mutex>
#include <queue>
#include <functional>

#include "modules/imodule.hpp"
#include "imoduleprotocol.hpp"
#include "tools/dfautomaton.hpp"

class IAccessPointModule;
class IDoorModule;
class IAuthModule;
class IMonitorModule;
class ILoggerModule;

class ModuleProtocol : public IModuleProtocol
{
    static const int AuthRequestValidity;
    typedef std::function<void (IModule*)> RegisterFunc;

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
    void    buildRegistrationHandler();
    void    buildAuthLogic();

private:
    AuthRequest::Uid                            _authCounter;
    std::map<AuthRequest::Uid, AuthRequest>     _requests;
    std::mutex                                  _commandsMutex;
    std::queue<ICommand::Ptr>                   _commands;
    DFAutomaton<AuthRequest, int, int>          _authLogic;

private:
    std::map<IModule::ModuleType, RegisterFunc> _registrationHandler;
    std::list<ILoggerModule*>                   _loggerModules;
    std::list<IMonitorModule*>                  _monitorModules;
    std::map<std::string, IDoorModule*>         _doorModules;
    std::map<std::string, IAccessPointModule*>  _apModules;
    IAuthModule*                                _authModule;
};

#endif // MODULEPROTOCOL_HPP
