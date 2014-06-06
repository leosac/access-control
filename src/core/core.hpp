/**
 * \file core.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief module managing class
 */

#ifndef CORE_HPP
#define CORE_HPP

#include <string>
#include <queue>
#include <list>
#include <map>
#include <atomic>
#include <mutex>

#include "icore.hpp"
#include "modulemanager.hpp"
#include "hardware/hwmanager.hpp"
#include "signal/isignalcallback.hpp"
#include "tools/runtimeoptions.hpp"
#include "config/xmlconfig.hpp"
#include "modules/imodule.hpp"

class ILoggerModule;
class IAuthModule;

class Core : public ICore, public ISignalCallback, public IXmlSerializable
{
    static const int IdleSleepTimeMs = 5;
    typedef void (Core::*RegisterFunc)(IModule*);

public:
    explicit Core(RuntimeOptions& options);
    ~Core() = default;

    Core(const Core& other) = delete;
    Core& operator=(const Core& other) = delete;

public:
    virtual IHWManager& getHWManager() override;
    virtual void        handleSignal(int signal) override;
    virtual void        serialize(ptree& node) override;
    virtual void        deserialize(const ptree& node) override;

public:
    void    run();

private:
    void    registerModule(IModule* module);
    void    registerDoorModule(IModule* module);
    void    registerAccessPointModule(IModule* module);
    void    registerAuthModule(IModule* module);
    void    registerLoggerModule(IModule* module);
    void    registerActivityMonitorModule(IModule* module);

private:
    RuntimeOptions&                             _options;
    HWManager                                   _hwManager;
    XmlConfig                                   _coreConfig;
    XmlConfig                                   _hwconfig;
    ModuleManager                               _moduleMgr;
    std::atomic<bool>                           _isRunning;
    std::list<std::string>                      _libsDirectories;
    std::map<IModule::ModuleType, RegisterFunc> _registrationHandler;
    std::list<ILoggerModule*>                   _loggerModules;
    IAuthModule*                                _authModule;
};

#endif // CORE_HPP
