/**
 * \file modulemanager.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief ModuleManager class declaration
 */

#ifndef MODULEMANAGER_HPP
#define MODULEMANAGER_HPP

#include <string>
#include <map>
#include <list>

#include "icore.hpp"
#include "modules/imodule.hpp"

class ILoggerModule;
class DynamicLibrary;

class ModuleManager
{
public:
    typedef struct s_module {
        std::string libname;
        IModule*    instance;
    } Module;

public:
    explicit ModuleManager();
    ~ModuleManager() = default;

    ModuleManager(const ModuleManager& other) = delete;
    ModuleManager& operator=(const ModuleManager& other) = delete;

public:
    void        loadLibraries(const std::list<std::string>& directories);
    void        unloadLibraries();
    IModule*    loadModule(ICore& core, const std::string& libname, const std::string& alias = std::string());
    void        unloadModules();

public:
    const std::map<std::string, Module>&    getModules() const;

private:
    void    loadLibrary(const std::string& path);

private:
    std::map<std::string, DynamicLibrary*>      _dynlibs;
    std::map<std::string, Module>               _modules;
};

#endif // MODULEMANAGER_HPP
