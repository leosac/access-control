/**
 * \file ScopeLock.h
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief Utility class for automatic mutex lock
 */

#ifndef SCOPELOCK_H
#define SCOPELOCK_H

#include "mutex.h"

class ScopeLock
{
public:
    ScopeLock(Mutex& mutex);
    ~ScopeLock();

private:
    ScopeLock(const ScopeLock& other);
    ScopeLock& operator=(const ScopeLock& other);

private:
    Mutex&    _mutex;
};

#endif // SCOPELOCK_H
