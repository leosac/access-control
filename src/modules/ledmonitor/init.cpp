/**
 * \file init.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief ledmonitor init function for returning a module class to the core
 */

#include "ledmonitormodule.hpp"

#ifndef MODULE_PUBLIC
#   ifdef ledmonitor_EXPORTS
#       define MODULE_PUBLIC __attribute__((visibility("default")))
#   elif defined(_WIN32)
#       define MODULE_PUBLIC
#   else
#       define MODULE_PUBLIC
#   endif
#endif

extern "C" MODULE_PUBLIC IModule* getNewModuleInstance(ICore& core, const std::string& name)
{
    return (new LedMonitorModule(core, name));
}
