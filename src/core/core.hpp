/**
 * \file core.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief module managing class
 */

#ifndef CORE_HPP
#define CORE_HPP

#include <mutex>
#include <list>
#include <map>

#include "event.hpp"
#include "modules/imodule.hpp"
#include "hardware/ihwmanager.hpp"
#include "signal/isignalcallback.hpp"

class DynamicLibrary;

class Core : public ISignalCallback
{
    static const int IdleSleepTimeMs = 1000;
public:
    Core();
    ~Core();

private:
    Core(const Core& other);
    Core& operator=(const Core& other);

public:
    void    handleSignal(int signal); // Inherited from ISignalCallback

public:
    void    run(const std::list<std::string>& args);

private:
    bool    parseArguments();
    void    load();
    void    unload();
    void    loadLibraries();
    void    unloadLibraries();
    bool    loadModule(const std::string& libname, const std::string& alias);
    void    dispatchEvent(const Event& event);

private:
    std::list<std::string>                  _args;
    std::mutex                              _runMutex;
    bool                                    _isRunning;
    IHWManager*                             _hwManager;
    std::map<std::string, IModule*>         _modules;
    std::map<std::string, DynamicLibrary*>  _dynlibs;
    std::list<std::string>                  _libsDirectories;
    std::list<IModule*>                     _loggerModules;
};

#endif // CORE_HPP
