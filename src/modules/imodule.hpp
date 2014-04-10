/**
 * \file imodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief module class interface
 */

#ifndef IMODULE_HPP
#define IMODULE_HPP

#include "core/event.hpp"

#include <string>

class IModule
{
public:
    typedef IModule* (*InitFunc)();
    enum Type {
        Door,
        AccessPoint,
        Authentication,
        Logger,
        ActivityMonitor
    };

public:
    virtual ~IModule() {}
    virtual void                sendEvent(const Event& event) = 0;
    virtual Type                getType() const = 0;
    virtual const std::string&  getVersionString() const = 0;
};

#endif // IMODULE_HPP
