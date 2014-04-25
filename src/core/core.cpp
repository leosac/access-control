/**
 * \file core.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief module managing class
 */

#include "core.hpp"

#include <thread>
#include <iostream>

#include "osac.hpp"
#include "signal/signalhandler.hpp"
#include "hardware/hwmanager.hpp"
#include "dynlib/dynamiclibrary.hpp"
#include "tools/unixfs.hpp"

#include "exception/signalexception.hpp"
#include "exception/dynlibexception.hpp"
#include "exception/moduleexception.hpp"

const int Core::IdleSleepTimeMs;

Core::Core()
:   _isRunning(false),
    _hwManager(nullptr),
    _authModule(nullptr)
{
    _libsDirectories.push_back(UnixFs::getCWD());
    _registrationHandler[IModule::Door] = &Core::registerDoorModule;
    _registrationHandler[IModule::AccessPoint] = &Core::registerAccessPointModule;
    _registrationHandler[IModule::Auth] = &Core::registerAuthModule;
    _registrationHandler[IModule::Logger] = &Core::registerLoggerModule;
    _registrationHandler[IModule::ActivityMonitor] = &Core::registerActivityMonitorModule;
}

Core::~Core() {}

Core::Core(const Core& /*other*/) {}

Core& Core::operator=(const Core& /*other*/) {return (*this);}

void Core::handleSignal(int /*signal*/)
{
    std::lock_guard<std::mutex> lg(_runMutex);
    if (_isRunning)
    {
        dispatchEvent(Event("caught signal", "Core"));
        _isRunning = false;
    }
}

void Core::run(const std::list<std::string>& args)
{
    _args = args;
    if (!parseArguments())
        return;
    load();
    dispatchEvent(Event("starting", "Core"));
    {
        std::lock_guard<std::mutex> lg(_runMutex);
        _isRunning = true;
        while (_isRunning)
        {
            _runMutex.unlock();
            dispatchEvent(Event("alive", "Core"));
            std::this_thread::sleep_for(std::chrono::milliseconds(IdleSleepTimeMs));
            _runMutex.lock();
        }
    }
    dispatchEvent(Event("exiting", "Core"));
    unload();
}

bool Core::parseArguments()
{
    for (auto arg : _args)
    {
        if (arg == "--version")
        {
            std::cout << "Version: " << OSAC::getVersionString() << std::endl;
            return (false);
        }
    }
    return (true);
}

void Core::load()
{
#ifndef NO_HW
    _hwManager = new HWManager;
#endif

    loadLibraries();

    // TODO load modules
    for (auto lib : _dynlibs)
        loadModule(lib.first, lib.first + "-test"); // FIXME Debug

    debugPrintModules();

    try
    {
        SignalHandler::registerCallback(this);
    }
    catch (const SignalException& e)
    {
        std::cerr << e.what() << std::endl;
    }
#ifndef NO_HW
    _hwManager->start();
#endif
}

void Core::unload()
{
#ifndef NO_HW
    _hwManager->stop();
#endif

    for (auto module : _modules)
        delete module.second;
    _modules.clear();

    unloadLibraries();

#ifndef NO_HW
    delete _hwManager;
    _hwManager = nullptr;
#endif
}

void Core::loadLibraries()
{
    std::string     libname;
    DynamicLibrary* lib;

    _libsDirectories.push_back("modules/rpleth");
    _libsDirectories.push_back("modules/journal");

    for (auto folder : _libsDirectories)
    {
        UnixFs::FileList fl = UnixFs::listFiles(folder, ".so");
        for (auto path : fl)
        {
            libname = UnixFs::stripPath(path);
            if (_dynlibs[libname] != nullptr)
            {
                std::cerr << "module already loaded (" << path << ')' << std::endl;
                continue;
            }
            lib = new DynamicLibrary(path);
            try
            {
                lib->open();
            }
            catch (const DynLibException& e)
            {
                std::cerr << e.what() << std::endl;
                delete lib;
            }
            _dynlibs[libname] = lib;
        }
    }
    debugPrintLibs(); // FIXME Debug
}

void Core::unloadLibraries()
{
    for (auto lib : _dynlibs)
    {
        try
        {
            lib.second->close();
        }
        catch (const DynLibException& e)
        {
            std::cerr << e.what() << std::endl;
        }
        delete lib.second;
    }
}

bool Core::loadModule(const std::string& libname, const std::string& alias)
{
    DynamicLibrary*     lib = _dynlibs[libname];
    IModule::InitFunc   func;
    IModule*            module = nullptr;

    if (!lib)
        return (false);
    if (_modules[alias])
        return (false);
    try
    {
        void* s = lib->getSymbol("getNewModuleInstance");
        *reinterpret_cast<void**>(&func) = s;
        module = func();
    }
    catch (const DynLibException& e)
    {
        std::cerr << e.what() << std::endl;
        delete module; // FIXME is it safe ?
        return (false);
    }
    registerModule(module, alias);
    return (true);
}

void Core::dispatchEvent(const Event& event)
{
    for (auto logger : _loggerModules)
        logger->sendEvent(event);
}

void Core::debugPrintLibs()
{
    std::cout << "Libs:" << std::endl;
    for (auto lib : _dynlibs)
        std::cout << "-> " << lib.first << std::endl;
}

void Core::debugPrintModules()
{
    std::cout << "Loaded modules:" << std::endl;
    for (auto module : _modules)
        std::cout << "-> " << module.first << std::endl;
}

void Core::registerModule(IModule* module, const std::string& alias)
{
    RegisterFunc    func = _registrationHandler[module->getType()];

    if (!func)
        throw (ModuleException("Unknown module type"));
    ((*this).*func)(module);
    _modules[alias] = module;
}

void Core::registerDoorModule(IModule* /*module*/)
{
    // TODO
}

void Core::registerAccessPointModule(IModule* /*module*/)
{
    // TODO
}

void Core::registerAuthModule(IModule* module)
{
    IAuthModule*    auth;

    if (!(auth = dynamic_cast<IAuthModule*>(module)))
        throw (ModuleException("Invalid Auth module"));
    if (_authModule)
        throw (ModuleException("Replacing existing Auth module"));
    _authModule = auth;
}

void Core::registerLoggerModule(IModule* module)
{
    IEventListenerModule*   logger;

    if (!(logger = dynamic_cast<IEventListenerModule*>(module)))
        throw (ModuleException("Invalid Logger module"));
    _loggerModules.push_back(logger);
}

void Core::registerActivityMonitorModule(IModule* /*module*/)
{
    // TODO
}
