/**
 * \file core.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief module managing class
 */

#include "core.hpp"

#include <thread>
#include <string>

#include "osac.hpp"
#include "signal/signalhandler.hpp"
#include "hardware/hwmanager.hpp"
#include "dynlib/dynamiclibrary.hpp"
#include "tools/unixfs.hpp"
#include "tools/unlock_guard.hpp"
#include "tools/log.hpp"

#include "exception/coreexception.hpp"
#include "exception/signalexception.hpp"
#include "exception/dynlibexception.hpp"

const int Core::IdleSleepTimeMs;

Core::Core(RuntimeOptions& options)
:   _options(options),
    _coreConfig(options.getParam("configfile"), *this),
    _isRunning(false),
    _hwManager(nullptr),
    _authModule(nullptr)
{
    _registrationHandler[IModule::ModuleType::Door] = &Core::registerDoorModule;
    _registrationHandler[IModule::ModuleType::AccessPoint] = &Core::registerAccessPointModule;
    _registrationHandler[IModule::ModuleType::Auth] = &Core::registerAuthModule;
    _registrationHandler[IModule::ModuleType::Logger] = &Core::registerLoggerModule;
    _registrationHandler[IModule::ModuleType::ActivityMonitor] = &Core::registerActivityMonitorModule;
}

void Core::notify(const Event& event)
{
    std::lock_guard<std::mutex> lg(_eventQueueMutex);
    _eventQueue.push(event);
}

IHWManager& Core::getHWManager()
{
    return (*_hwManager);
}

void Core::handleSignal(int signal)
{
    if (_isRunning)
    {
        LOG() << "caught signal (" << signal << ')';
        _isRunning = false;
    }
}

void Core::serialize(boost::property_tree::ptree& node)
{
    for (const auto& dir : _libsDirectories)
        node.add("core.plugindir", dir);
    for (const auto& module : _modules)
    {
        boost::property_tree::ptree& child = node.add("core.module", std::string());

        child.put("<xmlattr>.file", module.second.libname);
        child.put("alias", module.first);
        module.second.instance->serialize(child);
        delete module.second.instance;
    }
    _modules.clear();
    unloadLibraries();
}

void Core::deserialize(boost::property_tree::ptree& node)
{
    for (const auto& v : node.get_child("core"))
    {
        if (v.first == "plugindir")
            _libsDirectories.push_back(v.second.data());
    }
    loadLibraries();
    _hwManager = new HWManager;
    for (auto& v : node.get_child("core"))
    {
        if (v.first == "module")
        {
            IModule* module = loadModule(v.second.get<std::string>("<xmlattr>.file", "default"), v.second.get<std::string>("alias"));
            module->deserialize(v.second);
        }
    }
    if (!_authModule)
        throw (CoreException("No auth module loaded"));
    debugPrintModules();
}

void Core::run()
{
    _coreConfig.deserialize();
    LOG() << "starting core loop";
    SignalHandler::registerCallback(this);
    _hwManager->start();
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
    LOG() << "exiting core loop";
    _hwManager->stop();
    _coreConfig.serialize();
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
                throw (CoreException("module already loaded (" + path + ')'));
            lib = new DynamicLibrary(path);
            try {
                lib->open();
            }
            catch (const DynLibException& e) {
                delete lib;
                throw (CoreException(e.what()));
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
        try {
            lib.second->close();
        }
        catch (const DynLibException& e) {
            throw (CoreException(e.what()));
        }
        delete lib.second;
    }
}

IModule* Core::loadModule(const std::string& libname, const std::string& alias)
{
    DynamicLibrary*     lib = _dynlibs.at(libname);
    IModule::InitFunc   func;
    IModule*            module = nullptr;

    if (!lib)
        throw (CoreException("Invalid source library"));
    if (_modules.count(alias) > 0)
        throw (CoreException("A library named \'" + alias + "\' already exists"));
    try {
        void* s = lib->getSymbol("getNewModuleInstance");
        *reinterpret_cast<void**>(&func) = s;
        if (alias.empty())
            module = func(*this, libname + "-default");
        else
            module = func(*this, alias);
    }
    catch (const DynLibException& e) {
        delete module; // FIXME is it safe ?
        throw (CoreException(e.what()));
    }
    registerModule(module, libname, module->getName());
    return (module);
}

void Core::processEvent(const Event& event)
{
    IModule*    dest = nullptr;
    IModule*    source = nullptr;

    LOG() << "Processing event:\n    from:" << event.source << "\n    to:" << event.destination << "\n    msg:" << event.message;
    if (event.source.empty())
        throw (CoreException("No source specified from Event"));
    else if (event.source != "Core")
    {
        if (!(source = _modules[event.source].instance))
            throw (CoreException("Invalid"));
        if (source->getType() == IModule::ModuleType::AccessPoint)
        {
            AuthRequest ar(event.destination, event.message);

            if (!(dest = _modules[event.destination].instance))
                throw (CoreException("bad destination"));
            LOG() << "new AR pushed: id=" << ar.getUid() << " data=" << ar.getInfo();
            _authRequests.emplace(std::make_pair(ar.getUid(), ar));
            dest->notify(Event(std::to_string(ar.getUid()) + " request " + ar.getInfo()));
        }
        else if (source->getType() == IModule::ModuleType::Auth)
        {
            std::istringstream  iss(event.message);
            std::string         uidstr;
            AuthRequest::Uid    uid;

            iss >> uidstr;
            uid = std::stoi(uidstr);
            if (_authRequests.count(uid) > 0)
            {
                AuthRequest&    ar = _authRequests.at(uid);
                std::string     rslt;

                if (!(dest = _modules[ar.getTarget()].instance))
                    throw (CoreException("bad destination"));
                iss >> rslt;
                if (rslt == "granted")
                {
                    LOG() << "access granted for " << uidstr;
                    ar.grant(true);
                    dest->notify(Event(uidstr + " open"));
                }
                else if (rslt == "denied")
                {
                    LOG() << "access denied for " << uidstr;
                    _authRequests.erase(uid);
                }
                else
                    LOG() << "bad status from auth: " << rslt;
            }
            else
                throw (CoreException("bad uid from auth"));
        }
        else if (source->getType() == IModule::ModuleType::Door)
        {
            std::istringstream  iss(event.message);
            std::string         uidstr;
            AuthRequest::Uid    uid;

            iss >> uidstr;
            uid = std::stoi(uidstr);

            if (_authRequests.count(uid) > 0)
            {
                AuthRequest&    ar = _authRequests.at(uid);
                std::string     rslt;

                iss >> rslt;
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
    LOG() << "Libs: (total " << _dynlibs.size() << ")";
    for (auto& lib : _dynlibs)
        LOG() << "-> " << lib.first;
}

void Core::debugPrintModules()
{
    LOG() << "Loaded modules: (total " << _modules.size() << ")";
    for (auto& module : _modules)
        LOG() << "-> " << module.first;
}

void Core::registerModule(IModule* module, const std::string& libname, const std::string& alias)
{
    RegisterFunc    func = _registrationHandler[module->getType()];

    if (!func)
        throw (CoreException("Unknown module type"));
    ((*this).*func)(module);
    _modules[alias] = { libname, module };
}

void Core::registerDoorModule(IModule* /*module*/) {}

void Core::registerAccessPointModule(IModule* /*module*/) {}

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

void Core::registerActivityMonitorModule(IModule* /*module*/) {}
