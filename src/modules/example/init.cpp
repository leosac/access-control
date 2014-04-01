/**
 * \file init.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief example init function for returning a moduleloader class to the core
 */

#include "examplemoduleloader.hpp"

// FIXME Add correct support for -fvisibility=hidden
#ifndef MODULE_PUBLIC
#   ifdef example_EXPORTS
#       define MODULE_PUBLIC __attribute__((visibility("default")))
#   elif defined(_WIN32)
#       define MODULE_PUBLIC
#   endif
#endif

extern "C" MODULE_PUBLIC IModuleLoader* getLoader()
{
    return (new ExampleModuleLoader);
}
