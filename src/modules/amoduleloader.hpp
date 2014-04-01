/**
 * \file amoduleloader.hpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief module loader class containing basic informations
 */

#ifndef AMODULELOADER_HPP
#define AMODULELOADER_HPP

#include <string>

#include "imoduleloader.hpp"

class AModuleLoader : public IModuleLoader
{
public:
    AModuleLoader(const std::string& name, int major, int minor, int patch);
    virtual ~AModuleLoader();

private:
    AModuleLoader(const AModuleLoader& other);
    AModuleLoader& operator=(const AModuleLoader& other);

public:
    virtual IModule*    instanciateModule() const = 0;

public:
    const std::string&  getModuleName() const;
    const std::string&  getVersionString() const;

private:
    const std::string _name;
    const std::string _version;
};

#endif // AMODULELOADER_HPP
