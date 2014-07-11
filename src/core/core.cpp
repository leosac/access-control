/**
 * \file core.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief module managing class
 */

#include "core.hpp"

#include <thread>

#include "hardware/hwmanager.hpp"
#include "tools/signalhandler.hpp"
#include "tools/log.hpp"

#include "exception/coreexception.hpp"
#include "exception/signalexception.hpp"

const int Core::IdleSleepTimeMs;

Core::Core(RuntimeOptions& options)
:   _options(options),
    _hwManager(),
    _coreConfig(options.getParam("corecfg"), *this),
    _hwconfig(options.getParam("hwcfg"), _hwManager),
    _isRunning(false)
{}

IHWManager& Core::getHWManager()
{
    return (_hwManager);
}

ModuleProtocol& Core::getModuleProtocol()
{
    return (_authProtocol);
}

void Core::handleSignal(int signal)
{
    _isRunning = false;
    LOG() << "caught signal (" << signal << ')';
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
        module.second.instance->serialize(child.put("properties", std::string()));
    }
    _networkConfig.serialize(core.put("network", std::string()));
    node.put_child("core", core);
    _moduleMgr.unloadModules();
    _moduleMgr.unloadLibraries();
}

void Core::deserialize(const ptree& node)
{
    IModule*    module;

    _networkConfig.deserialize(node.get_child("core.network"));
    for (const auto& v : node.get_child("core"))
    {
        if (v.first == "plugindir")
            _libsDirectories.push_back(v.second.data());
    }
    _moduleMgr.loadLibraries(_libsDirectories);
    for (const auto& v : node.get_child("core"))
    {
        if (v.first == "module")
        {
            module = _moduleMgr.loadModule(*this, v.second.get<std::string>("<xmlattr>.file", "default"), v.second.get<std::string>("alias"));
            _authProtocol.registerModule(module);
            module->deserialize(v.second.get_child("properties"));
        }
    }
}

void Core::run()
{
    try {
        _isRunning = true;
        _hwManager.setStateHook(HWManager::HookType::DHCP, [this] (bool state) { _networkConfig.setDHCP(state); } );
        _hwManager.setStateHook(HWManager::HookType::DefaultIp, [this] (bool state) { _networkConfig.setCustomIP(state); } );
        _hwconfig.deserialize();
        LOG() << "devices are up";
        _hwManager.start();
        LOG() << "hwmanager started";
        _coreConfig.deserialize();
        LOG() << "core config loaded";
        _networkConfig.reload();
        LOG() << "network loaded";
        SignalHandler::registerCallback([this] (int signal) { handleSignal(signal); } );
        LOG() << "starting core loop";
        while (_isRunning)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(IdleSleepTimeMs));
            // NOTE watch config files here, if needed
            _hwManager.sync();
            _authProtocol.sync();
        }
        LOG() << "exiting core loop";
        _hwManager.stop();
        LOG() << "hwmanager stopped";
        _coreConfig.serialize();
        LOG() << "core config unloaded";
        _hwconfig.serialize();
        LOG() << "devices are down";
    }
    catch (const LEOSACException& e) {
        std::cerr << "Fatal exception: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Unexpected exception caught: " << e.what() << std::endl;
    }
}
