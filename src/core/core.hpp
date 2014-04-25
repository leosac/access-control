/**
 * \file core.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief module managing class
 */

#ifndef CORE_HPP
#define CORE_HPP

#include <atomic>
#include <list>
#include <map>

#include "event.hpp"
#include "modules/imodule.hpp"
#include "modules/ieventlistenermodule.hpp"
#include "modules/iauthmodule.hpp"
#include "hardware/ihwmanager.hpp"
#include "signal/isignalcallback.hpp"

class DynamicLibrary;

class Core : public ISignalCallback
{
    static const int IdleSleepTimeMs = 10000;
    typedef void (Core::*RegisterFunc)(IModule*);

public:
    Core();
    ~Core();

private:
    Core(const Core& other);
    Core& operator=(const Core& other);

public:
    void    handleSignal(int signal); // Inherited from ISignalCallback
    void    run(const std::list<std::string>& args);

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
    IHWManager*                             _hwManager;
    std::list<std::string>                  _libsDirectories;
    std::map<std::string, DynamicLibrary*>  _dynlibs;
    std::map<std::string, IModule*>         _modules;
    std::map<IModule::Type, RegisterFunc>   _registrationHandler;
    std::list<IEventListenerModule*>        _loggerModules;
    IAuthModule*                            _authModule;
};

#endif // CORE_HPP
