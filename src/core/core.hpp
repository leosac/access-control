/**
 * \file core.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief module managing class
 */

#ifndef CORE_HPP
#define CORE_HPP

#include <queue>
#include <list>
#include <map>
#include <atomic>
#include <mutex>

#include "event.hpp"
#include "icore.hpp"
#include "authrequest.hpp"
#include "modules/imodule.hpp"
#include "modules/ieventlistener.hpp"
#include "modules/iauthmodule.hpp"
#include "hardware/ihwmanager.hpp"
#include "signal/isignalcallback.hpp"
#include "runtimeoptions.hpp"
#include "config/coreconfig.hpp"

class DynamicLibrary;

class Core : public ISignalCallback, public ICore
{
    static const int IdleSleepTimeMs = 5;
    typedef void (Core::*RegisterFunc)(IModule*);

public:
    explicit Core(RuntimeOptions& options);
    ~Core() = default;

    Core(const Core& other) = delete;
    Core& operator=(const Core& other) = delete;

public:
    virtual void        handleSignal(int signal) override;
    virtual void        notify(const Event& event) override;
    virtual IHWManager* getHWManager() override;

public:
    void    run();

private:
    void    load();
    void    unload();
    void    loadLibraries();
    void    unloadLibraries();
    void    loadConfig();
    void    unloadConfig();
    bool    loadModule(const std::string& libname, const std::string& alias = std::string());
    void    processEvent(const Event& event);
    void    debugPrintLibs();
    void    debugPrintModules();

private:
    void    registerModule(IModule* module, const std::string& alias);
    void    registerDoorModule(IModule* module);
    void    registerAccessPointModule(IModule* module);
    void    registerAuthModule(IModule* module);
    void    registerLoggerModule(IModule* module);
    void    registerActivityMonitorModule(IModule* module);

private:
    RuntimeOptions&                             _options;
    CoreConfig                                  _coreConfig;
    std::atomic<bool>                           _isRunning;
    IHWManager*                                 _hwManager;
    std::list<std::string>                      _libsDirectories;
    std::map<std::string, DynamicLibrary*>      _dynlibs;
    std::map<std::string, IModule*>             _modules;
    std::map<IModule::ModuleType, RegisterFunc> _registrationHandler;
    std::list<IEventListener*>                  _loggerModules;
    IAuthModule*                                _authModule;
    std::priority_queue<Event>                  _eventQueue;
    std::mutex                                  _eventQueueMutex;
    std::map<AuthRequest::Uid, AuthRequest>     _authRequests;
};

#endif // CORE_HPP
