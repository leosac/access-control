/**
 * \file core.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief module managing class
 */

#include "core.hpp"

#include <thread>

#include "osac.hpp"
#include "signal/signalhandler.hpp"
#include "hardware/hwmanager.hpp"
#include "tools/log.hpp"

#include "exception/coreexception.hpp"
#include "exception/signalexception.hpp"

#include "modules/iauthmodule.hpp"
#include "modules/iloggermodule.hpp"

const int Core::IdleSleepTimeMs;

Core::Core(RuntimeOptions& options)
:   _options(options),
    _hwManager(),
    _coreConfig(options.getParam("corecfg"), *this),
    _hwconfig(options.getParam("hwcfg"), _hwManager),
    _isRunning(false),
    _authModule(nullptr)
{
    _registrationHandler[IModule::ModuleType::Door] = &Core::registerDoorModule;
    _registrationHandler[IModule::ModuleType::AccessPoint] = &Core::registerAccessPointModule;
    _registrationHandler[IModule::ModuleType::Auth] = &Core::registerAuthModule;
    _registrationHandler[IModule::ModuleType::Logger] = &Core::registerLoggerModule;
    _registrationHandler[IModule::ModuleType::ActivityMonitor] = &Core::registerActivityMonitorModule;
}

IHWManager& Core::getHWManager()
{
    return (_hwManager);
}

void Core::handleSignal(int signal)
{
    if (_isRunning)
    {
        LOG() << "caught signal (" << signal << ')';
        _isRunning = false;
    }
}

void Core::serialize(ptree& node)
{
    ptree core;

    for (const auto& dir : _libsDirectories)
        core.add("plugindir", dir);
    for (const auto& module : _moduleMgr.getModules())
    {
        ptree& child = core.add("module", std::string());

        child.put("<xmlattr>.file", module.second.libname);
        child.put("alias", module.first);
        module.second.instance->serialize(child);
    }
    node.put_child("core", core);
    _moduleMgr.unloadModules();
    _moduleMgr.unloadLibraries();
}

void Core::deserialize(const ptree& node)
{
    IModule*    module;

    for (const auto& v : node.get_child("core"))
    {
        if (v.first == "plugindir")
            _libsDirectories.push_back(v.second.data());
    }
    _moduleMgr.loadLibraries(_libsDirectories);
    for (auto& v : node.get_child("core"))
    {
        if (v.first == "module")
        {
            module = _moduleMgr.loadModule(*this, v.second.get<std::string>("<xmlattr>.file", "default"), v.second.get<std::string>("alias"));
            registerModule(module);
            module->deserialize(v.second);
        }
    }
    if (!_authModule)
        throw (CoreException("No auth module loaded"));
}

void Core::run()
{
    _hwconfig.deserialize();
    LOG() << "devices are up";
    _hwManager.start();
    LOG() << "hwmanager started";
    _coreConfig.deserialize();
    LOG() << "core config loaded";
    SignalHandler::registerCallback(this);
    LOG() << "starting core loop";
    _isRunning = true;
    while (_isRunning)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(IdleSleepTimeMs));
        // TODO
    }
    LOG() << "exiting core loop";
    _hwManager.stop();
    _coreConfig.serialize();
    _hwconfig.serialize();
}

void Core::registerModule(IModule* module)
{
    RegisterFunc    func = _registrationHandler[module->getType()];

    if (!func)
        throw (CoreException("Unknown module type"));
    ((*this).*func)(module);
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
    ILoggerModule* logger;

    if (!(logger = dynamic_cast<ILoggerModule*>(module)))
        throw (CoreException("Invalid Logger module"));
    _loggerModules.push_back(logger);
}

void Core::registerActivityMonitorModule(IModule* /*module*/) {}
