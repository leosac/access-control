/**
 * \file init.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief init function for returning a module class to the core
 */

#include "journallogger.hpp"

// FIXME Add correct support for -fvisibility=hidden
#ifndef MODULE_PUBLIC
#   ifdef journal_EXPORTS
#       define MODULE_PUBLIC __attribute__((visibility("default")))
#   elif defined(_WIN32)
#       define MODULE_PUBLIC
#   endif
#endif

extern "C" MODULE_PUBLIC IModule* getNewModuleInstance(ICore& /*core*/)
{
    return (new JournalLogger);
}
