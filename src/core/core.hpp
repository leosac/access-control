/**
 * \file core.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief module managing class
 */

#ifndef CORE_HPP
#define CORE_HPP

#include <queue>
#include <list>
#include <map>
#include <atomic>
#include <mutex>

#include "event.hpp"
#include "icore.hpp"
#include "authrequest.hpp"
#include "modules/imodule.hpp"
#include "modules/ieventlistener.hpp"
#include "modules/iauthmodule.hpp"
#include "hardware/hwmanager.hpp"
#include "signal/isignalcallback.hpp"
#include "runtimeoptions.hpp"
#include "config/xmlconfig.hpp"

class DynamicLibrary;

class Core : public ICore, public ISignalCallback, public IXmlSerializable
{
    static const int IdleSleepTimeMs = 5;
    typedef void (Core::*RegisterFunc)(IModule*);
    typedef struct s_module {
        std::string libname;
        IModule*    instance;
    } Module;

public:
    explicit Core(RuntimeOptions& options);
    ~Core() = default;

    Core(const Core& other) = delete;
    Core& operator=(const Core& other) = delete;

public:
    virtual void        notify(const Event& event) override;
    virtual IHWManager& getHWManager() override;
    virtual void        handleSignal(int signal) override;
    virtual void        serialize(boost::property_tree::ptree& node) override;
    virtual void        deserialize(const boost::property_tree::ptree& node) override;

public:
    void    run();

private:
    void        loadLibraries();
    void        unloadLibraries();
    IModule*    loadModule(const std::string& libname, const std::string& alias = std::string());
    void        processEvent(const Event& event);
    void        debugPrintLibs();
    void        debugPrintModules();

private:
    void    registerModule(IModule* module, const std::string& libname, const std::string& alias);
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

private:
    std::atomic<bool>                           _isRunning;
    std::list<std::string>                      _libsDirectories;
    std::map<std::string, DynamicLibrary*>      _dynlibs;
    std::map<std::string, Module>               _modules;
    std::map<IModule::ModuleType, RegisterFunc> _registrationHandler;
    std::list<IEventListener*>                  _loggerModules;
    IAuthModule*                                _authModule;
    std::priority_queue<Event>                  _eventQueue;
    std::mutex                                  _eventQueueMutex;
    std::map<AuthRequest::Uid, AuthRequest>     _authRequests;
};

#endif // CORE_HPP
