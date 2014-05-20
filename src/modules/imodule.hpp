/**
 * \file imodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief module class interface
 */

#ifndef IMODULE_HPP
#define IMODULE_HPP

#include "core/icore.hpp"
#include "config/ixmlserializable.hpp"

#include <string>

class IModule : public IEventListener, public IXmlSerializable
{
public:
    typedef IModule* (*InitFunc)(ICore&, const std::string&);
    enum class ModuleType : unsigned int {
        Door = 0,
        AccessPoint,
        Auth,
        Logger,
        ActivityMonitor
    };

public:
    virtual ~IModule() = default;
    virtual const std::string&  getName() const = 0;
    virtual ModuleType          getType() const = 0;
};

#endif // IMODULE_HPP
