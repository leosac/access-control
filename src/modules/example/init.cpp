/**
 * \file init.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief example init function for returning a module class to the core
 */

#include "examplemodule.hpp"

// FIXME Add correct support for -fvisibility=hidden
#ifndef MODULE_PUBLIC
#   ifdef example_EXPORTS
#       define MODULE_PUBLIC __attribute__((visibility("default")))
#   elif defined(_WIN32)
#       define MODULE_PUBLIC
#   endif
#endif

extern "C" MODULE_PUBLIC IModule* getNewModuleInstance()
{
    return (new ExampleModule);
}
