/**
 * \file scopelock.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief Utility class for automatic mutex lock
 */

#include "scopelock.hpp"

ScopeLock::ScopeLock(Mutex& mutex)
: _mutex(mutex)
{
    _mutex.lock();
}

ScopeLock::~ScopeLock()
{
    _mutex.unlock();
}

ScopeLock::ScopeLock(const ScopeLock& /*other*/) : _mutex(_mutex) {}

ScopeLock& ScopeLock::operator=(const ScopeLock& /*other*/)
{
    return (*this);
}
