/**
 * \file core.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief module managing class
 */

#ifndef CORE_HPP
#define CORE_HPP

#include <string>
#include <list>
#include <atomic>

#include "icore.hpp"
#include "config/xmlconfig.hpp"
#include "modulemanager.hpp"
#include "moduleprotocol/moduleprotocol.hpp"
#include "netconfig/networkconfig.hpp"
#include "hardware/hwmanager.hpp"
#include "tools/runtimeoptions.hpp"

class Core : public ICore, public IXmlSerializable
{
    static const int IdleSleepTimeMs = 5;

public:
    explicit Core(RuntimeOptions& options);
    ~Core() = default;

    Core(const Core& other) = delete;
    Core& operator=(const Core& other) = delete;

public:
    virtual IHWManager&     getHWManager() override;
    virtual ModuleProtocol& getModuleProtocol() override;
    virtual void            serialize(ptree& node) override;
    virtual void            deserialize(const ptree& node) override;

public:
    void    handleSignal(int signal);
    int     run();

public:
    void    setResetSwitch(bool enabled);

private:
    RuntimeOptions&         _options;
    HWManager               _hwManager;
    XmlConfig               _coreConfig;
    XmlConfig               _hwconfig;
    NetworkConfig           _networkConfig;
    ModuleManager           _moduleMgr;
    ModuleProtocol          _authProtocol;
    std::atomic<bool>       _isRunning;
    bool                    _resetSwitch;
    std::list<std::string>  _libsDirectories;
};

#endif // CORE_HPP
