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
#include "modules/imoduleloader.hpp"
#include "hardware/hwmanager.hpp"
#include "hardware/device/gpio.hpp" // FIXME Debug
#include "dynlib/dynamiclibrary.hpp"
#include "tools/unixfs.hpp"

const int Core::IdleSleepTimeMs;

Core::Core()
:   _isRunning(false),
    _hwManager(nullptr)
{
    _moduleDirectories.push_back(UnixFs::getCWD());
    _moduleDirectories.push_back("modules/example");
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

    for (auto folder : _moduleDirectories)
    {
        UnixFs::FileList fl = UnixFs::listFiles(folder, ".so");
        for (auto lib : fl)
            loadModule(lib, lib);
    }

    try
    {
        SignalHandler::registerCallback(this);
    }
    catch (const SignalException& e)
    {
        dispatchEvent(Event(e.what(), "Core"));
    }
}

void Core::unload()
{
    for (auto module : _modules)
        delete module.second;
    _modules.clear();

#ifndef NO_HW
    delete _hwManager;
    _hwManager = nullptr;
#endif
}

bool Core::loadModule(const std::string& path, const std::string& alias)
{
    DynamicLibrary          lib(UnixFs::getCWD() + '/' + path);
    IModuleLoader::InitFunc func;
    IModule*                module = nullptr;

    std::cout << "load " << path << " alias " << alias << std::endl;
    if (_modules[alias] != nullptr)
        return (false);
    try
    {
        IModuleLoader*          moduleloader;

        lib.open(DynamicLibrary::Now);
        void* s = lib.getSymbol("getLoader");
        *reinterpret_cast<void**>(&func) = s;
        moduleloader = func();
        std::cout << "Module " << moduleloader->getModuleName() << " loaded (v" << moduleloader->getVersionString() << ")" << std::endl;
        module = moduleloader->instanciateModule();
        delete moduleloader; // FIXME Debug
    }
    catch (const DynLibException& e)
    {
        dispatchEvent(Event(e.what(), "Core", Event::Error));
        if (module)
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
