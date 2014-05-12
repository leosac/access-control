/**
 * \file imodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief module class interface
 */

#ifndef IMODULE_HPP
#define IMODULE_HPP

#include "core/icore.hpp"

#include <string>

class IModule : public IEventListener
{
public:
    typedef IModule* (*InitFunc)(ICore&);
    enum class ModuleType : unsigned int {
        Door = 0,
        AccessPoint,
        Auth,
        Logger,
        ActivityMonitor
    };

public:
    virtual ~IModule() = default;
    virtual ModuleType          getType() const = 0;
    virtual const std::string&  getVersionString() const = 0;
};

#endif // IMODULE_HPP
