/**
 * \file imoduleloader.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief module loader interface
 */

#ifndef IMODULELOADER_HPP
#define IMODULELOADER_HPP

#include "imodule.hpp"

#include <string>

class IModuleLoader
{
public:
    typedef IModuleLoader* (*InitFunc)();

public:
    virtual ~IModuleLoader() {}
    virtual IModule*            instanciateModule() const = 0;
    virtual const std::string&  getModuleName() const = 0;
    virtual const std::string&  getVersionString() const = 0;
};

#endif // IMODULELOADER_HPP
