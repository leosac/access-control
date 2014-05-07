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
#include "modules/imodule.hpp"
#include "modules/ieventlistener.hpp"
#include "modules/iauthmodule.hpp"
#include "hardware/ihwmanager.hpp"
#include "signal/isignalcallback.hpp"

class DynamicLibrary;

class Core : public ISignalCallback, public ICore
{
    static const int IdleSleepTimeMs = 5;
    typedef void (Core::*RegisterFunc)(IModule*);

public:
    explicit Core();
    ~Core() = default;

    Core(const Core& other) = delete;
    Core& operator=(const Core& other) = delete;

public:
    void        handleSignal(int signal); // Inherited from ISignalCallback
    void        notify(const Event& event); // Inherited from IEventListener
    IHWManager* getHWManager(); // Inherited from ICore
    void        run(const std::list<std::string>& args);

private:
    bool    parseArguments();
    void    load();
    void    unload();
    void    loadLibraries();
    void    unloadLibraries();
    bool    loadModule(const std::string& libname, const std::string& alias);
    void    dispatchEvent(const Event& event);
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
    std::list<std::string>                  _args;
    std::atomic<bool>                       _isRunning;

private:
    IHWManager*                                 _hwManager;
    std::list<std::string>                      _libsDirectories;
    std::map<std::string, DynamicLibrary*>      _dynlibs;
    std::map<std::string, IModule*>             _modules;
    std::map<IModule::Type, RegisterFunc>       _registrationHandler;
    std::list<IEventListener*>                  _loggerModules;
    IAuthModule*                                _authModule;
    std::priority_queue<Event>                  _eventQueue;
    std::mutex                                  _eventQueueMutex;
};

#endif // CORE_HPP
