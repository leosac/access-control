/**
 * \file ScopeLock.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief Utility class for automatic mutex lock
 */

#include "ScopeLock.h"

ScopeLock::ScopeLock(Mutex& mutex)
: _mutex(mutex)
{
  _mutex.lock();
}

ScopeLock::~ScopeLock()
{
  _mutex.unlock();
}

ScopeLock::ScopeLock(const ScopeLock& other)
: _mutex(_mutex)
{
  static_cast<void>(other);
}

ScopeLock& ScopeLock::operator=(const ScopeLock& other)
{
  static_cast<void>(other);
  return (*this);
}
