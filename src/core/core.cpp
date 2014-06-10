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

using std::this_thread::sleep_for;
using std::chrono::milliseconds;

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

void Core::sendAuthRequest(const std::string& request)
{
    _authProtocol.createAuthRequest(request);
}

void Core::authorize(AuthRequest::Uid id, bool granted)
{
    // TODO
}

void Core::handleSignal(int signal)
{
    if (_isRunning)
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
            _authProtocol.registerModule(module);
            module->deserialize(v.second);
        }
    }
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
        sleep_for(milliseconds(IdleSleepTimeMs));
        _authProtocol.sync();
    }
    LOG() << "exiting core loop";
    _hwManager.stop();
    _coreConfig.serialize();
    _hwconfig.serialize();
}
