/**
 * \file core.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief module managing class
 */

#include "core.hpp"

#include <thread>
#include <iostream>
#include <sstream>
#include <string>

#include "osac.hpp"
#include "signal/signalhandler.hpp"
#include "hardware/hwmanager.hpp"
#include "dynlib/dynamiclibrary.hpp"
#include "tools/unixfs.hpp"
#include "tools/unlock_guard.hpp"

#include "exception/coreexception.hpp"
#include "exception/signalexception.hpp"
#include "exception/dynlibexception.hpp"

const int Core::IdleSleepTimeMs;

Core::Core()
:   _isRunning(false),
    _hwManager(nullptr),
    _authModule(nullptr)
{
    _libsDirectories.push_back(UnixFs::getCWD());
    _registrationHandler[IModule::ModuleType::Door] = &Core::registerDoorModule;
    _registrationHandler[IModule::ModuleType::AccessPoint] = &Core::registerAccessPointModule;
    _registrationHandler[IModule::ModuleType::Auth] = &Core::registerAuthModule;
    _registrationHandler[IModule::ModuleType::Logger] = &Core::registerLoggerModule;
    _registrationHandler[IModule::ModuleType::ActivityMonitor] = &Core::registerActivityMonitorModule;
    std::cout << "Open Source Access Controller v" << OSAC::getVersionString() << std::endl;
}

void Core::handleSignal(int signal)
{
    if (_isRunning)
    {
        notify(Event("caught signal (" + std::to_string(signal) + ')', "Core"));
        _isRunning = false;
    }
}

void Core::notify(const Event& event)
{
    std::lock_guard<std::mutex> lg(_eventQueueMutex);
    _eventQueue.push(event);
}

IHWManager* Core::getHWManager()
{
    return (_hwManager);
}

void Core::run()
{
    load();
    notify(Event("starting", "Core"));
    _isRunning = true;
    while (_isRunning)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(IdleSleepTimeMs));
        {
            std::lock_guard<std::mutex> lg(_eventQueueMutex);
            while (!_eventQueue.empty())
            {
                Event e = _eventQueue.top();

                _eventQueue.pop();
                unlock_guard<std::mutex> ulg(_eventQueueMutex);
                processEvent(e);
            }
        }
    }
    notify(Event("exiting", "Core"));
    unload();
}

void Core::load()
{
    _hwManager = new HWManager;

    loadLibraries();

    // TODO load modules
    for (auto& lib : _dynlibs)
        loadModule(lib.first);


    if (!_authModule)
        throw (CoreException("No auth module loaded"));

    debugPrintModules();

    try
    {
        SignalHandler::registerCallback(this);
    }
    catch (const SignalException& e)
    {
        std::cerr << e.what() << std::endl;
    }
    _hwManager->start();
}

void Core::unload()
{
    _hwManager->stop();

    for (auto& module : _modules)
        delete module.second;
    _modules.clear();

    unloadLibraries();

    delete _hwManager;
    _hwManager = nullptr;
}

void Core::loadLibraries()
{
    std::string     libname;
    DynamicLibrary* lib = nullptr;

    for (auto& folder : _libsDirectories)
    {
        UnixFs::FileList fl = UnixFs::listFiles(folder, ".so");
        for (auto& path : fl)
        {
            libname = UnixFs::stripPath(path);
            if (_dynlibs.count(libname) > 0)
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
    debugPrintLibs();
}

void Core::unloadLibraries()
{
    for (auto& lib : _dynlibs)
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
    DynamicLibrary*     lib = _dynlibs.at(libname);
    IModule::InitFunc   func;
    IModule*            module = nullptr;

    if (!lib)
        return (false);
    if (_modules.count(alias) > 0)
        return (false);
    try
    {
        void* s = lib->getSymbol("getNewModuleInstance");
        *reinterpret_cast<void**>(&func) = s;
        if (alias.empty())
            module = func(*this, libname + "-default");
        else
            module = func(*this, alias);
    }
    catch (const DynLibException& e)
    {
        std::cerr << e.what() << std::endl;
        delete module; // FIXME is it safe ?
        return (false);
    }
    registerModule(module, module->getName());
    return (true);
}

void Core::processEvent(const Event& event)
{
    IModule*    dest = nullptr;
    IModule*    source = nullptr;

    if (event.source.empty())
        throw (CoreException("No source specified from Event"));
    else if (event.source != "Core")
    {
        if (!(source = _modules[event.source]))
            throw (CoreException("Invalid"));
        if (source->getType() == IModule::ModuleType::AccessPoint)
        {
            AuthRequest ar(event.destination, event.message);

            if (!(dest = _modules[event.destination]))
                throw (CoreException("bad destination"));
            std::cout << "CORE::New AR pushed: id=" << ar.getUid() << std::endl; // DEBUG
            _authRequests.emplace(std::make_pair(ar.getUid(), ar));
            dest->notify(Event(std::to_string(ar.getUid()) + " request"));
        }
        else if (source->getType() == IModule::ModuleType::Auth)
        {
            std::stringstream   ss(event.message);
            std::string         uidstr;
            AuthRequest::Uid    uid;

            ss >> uidstr;
            uid = std::stoi(uidstr);
            if (_authRequests.count(uid) > 0)
            {
                AuthRequest&    ar = _authRequests.at(uid);
                std::string     rslt;

                if (!(dest = _modules[ar.getTarget()]))
                    throw (CoreException("bad destination"));
                ss >> rslt;
                if (rslt == "granted")
                {
                    ar.grant(true);
                    dest->notify(Event(uidstr + " open"));
                }
                else if (rslt == "denied")
                    _authRequests.erase(uid);
                else
                    std::cerr << "bad status from auth: " << rslt << std::endl;
            }
            else
                throw (CoreException("bad uid from auth"));
        }
        else if (source->getType() == IModule::ModuleType::Door)
        {
            std::stringstream   ss(event.message);
            std::string         uidstr;
            AuthRequest::Uid    uid;

            ss >> uidstr;
            uid = std::stoi(uidstr);

            if (_authRequests.count(uid) > 0)
            {
                AuthRequest&    ar = _authRequests.at(uid);
                std::string     rslt;

                ss >> rslt;
                if (rslt == "opened")
                    _authRequests.erase(uid);
                else if (rslt == "askauth")
                    _authModule->notify(Event(uidstr + " request " + ar.getInfo()));
            }
            else
                throw (CoreException("bad uid from auth"));
        }
    }
    for (auto& logger : _loggerModules)
        logger->notify(event);
}

void Core::debugPrintLibs()
{
    std::cout << "Libs: (" << _dynlibs.size() << " total)" << std::endl;
    for (auto& lib : _dynlibs)
        std::cout << "-> " << lib.first << std::endl;
}

void Core::debugPrintModules()
{
    std::cout << "Loaded modules: (" << _modules.size() << " total)" << std::endl;
    for (auto& module : _modules)
        std::cout << "-> " << module.first << std::endl;
}

void Core::registerModule(IModule* module, const std::string& alias)
{
    RegisterFunc    func = _registrationHandler[module->getType()];

    if (!func)
        throw (CoreException("Unknown module type"));
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
        throw (CoreException("Invalid Auth module"));
    if (_authModule)
        throw (CoreException("Replacing existing Auth module"));
    _authModule = auth;
}

void Core::registerLoggerModule(IModule* module)
{
    IEventListener* logger;

    if (!(logger = dynamic_cast<IEventListener*>(module)))
        throw (CoreException("Invalid Logger module"));
    _loggerModules.push_back(logger);
}

void Core::registerActivityMonitorModule(IModule* /*module*/)
{
    // TODO
}
