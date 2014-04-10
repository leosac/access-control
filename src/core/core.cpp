/**
 * \file core.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief module managing class
 */

#include "core.hpp"

#include <thread>
#include <iostream> // FIXME Debug

#include "osac.hpp"
#include "signal/signalhandler.hpp"
#include "exception/signalexception.hpp"
#include "exception/dynlibexception.hpp"
#include "modules/journallogger.hpp"
#include "hardware/hwmanager.hpp"
#include "hardware/device/gpio.hpp" // FIXME Debug
#include "dynlib/dynamiclibrary.hpp"
#include "tools/unixfs.hpp"

const int Core::IdleSleepTimeMs;

Core::Core()
:   _isRunning(false),
    _hwManager(nullptr)
{
    _libsDirectories.push_back(UnixFs::getCWD());
}

Core::~Core() {}

Core::Core(const Core& /*other*/) {}

Core& Core::operator=(const Core& /*other*/)
{
    return (*this);
}

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
    _runMutex.lock();
    _isRunning = true;
    while (_isRunning)
    {
        _runMutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(IdleSleepTimeMs));
        dispatchEvent(Event("alive", "Core"));
        _runMutex.lock();
    }
    _runMutex.unlock();
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

    if (!_args.size())
        throw (SignalException("Not enough arguments"));

    _gpio = _hwManager->reserveGPIO(std::stoi(_args.front()));
    _gpio->setDirection(GPIO::In);
    _gpio->setEdgeMode(GPIO::Rising);
    _gpio->startPolling();
#endif

    loadLibraries();

    try
    {
        SignalHandler::registerCallback(this);
    }
    catch (const SignalException& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void Core::unload()
{
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

    _libsDirectories.push_back("modules/example"); // FIXME Debug

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
        delete module;
        return (false);
    }
    _modules[alias] = module;
    return (true);
}

void Core::dispatchEvent(const Event& event)
{
    for (auto logger : _loggerModules)
        logger->sendEvent(event);
    if (_loggerModules.empty())
        std::cout << '[' << event.date.time_since_epoch().count() << ']' << " " << Event::getLogLevelString(event.logLevel) << " " << event.source << "::" << event.message<< std::endl;
}
