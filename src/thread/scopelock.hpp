/**
 * \file scopelock.hpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief Utility class for automatic mutex lock
 */

#ifndef SCOPELOCK_HPP
#define SCOPELOCK_HPP

#include "mutex.hpp"

class ScopeLock
{
public:
    ScopeLock(Mutex& mutex);
    ~ScopeLock();

private:
    ScopeLock(const ScopeLock& other);
    ScopeLock& operator=(const ScopeLock& other);

private:
    Mutex&  _mutex;
};

#endif // SCOPELOCK_HPP
