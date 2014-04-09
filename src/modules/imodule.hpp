/**
 * \file imodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief module class interface
 */

#ifndef IMODULE_HPP
#define IMODULE_HPP

#include "core/event.hpp"

class IModule
{
public:
    virtual ~IModule() {}
    virtual void    sendEvent(const Event& event) = 0;
};

#endif // IMODULE_HPP
