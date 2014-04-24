/**
 * \file ieventlistenermodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief interface for Logger and Activity Monitor classes
 */

#ifndef IEVENTLISTENERMODULE_HPP
#define IEVENTLISTENERMODULE_HPP

#include "imodule.hpp"
#include "core/event.hpp"

#include <string>

class IEventListenerModule : public IModule
{
public:
    virtual ~IEventListenerModule() {}
    virtual void    sendEvent(const Event& event) = 0;
};

#endif // IEVENTLISTENERMODULE_HPP
