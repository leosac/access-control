/**
 * \file Mutex.h
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief Object wrapper around pthread mutexes
 */

#include "mutex.h"

#include <cerrno>
#include "exception/mutexexception.hpp"

Mutex::Mutex()
{
  if ((_errNo = pthread_mutex_init(&_mutex, NULL)))
    throw (MutexException("pthread_mutex_init() failed", _errNo));
}

Mutex::~Mutex()
{
  if ((_errNo = pthread_mutex_destroy(&_mutex)))
    throw (MutexException("pthread_mutex_destroy() failed", _errNo));
}

Mutex::Mutex(const Mutex& other)
{
  static_cast<void>(other);
}

Mutex& Mutex::operator=(const Mutex& other)
{
  static_cast<void>(other);
  return (*this);
}

void Mutex::lock()
{
  if ((_errNo = pthread_mutex_lock(&_mutex)))
    throw (MutexException("pthread_mutex_lock() failed", _errNo));
}

void Mutex::unlock()
{
  if ((_errNo = pthread_mutex_unlock(&_mutex)))
    throw (MutexException("pthread_mutex_unlock() failed", _errNo));
}

bool Mutex::trylock()
{
  _errNo = pthread_mutex_trylock(&_mutex);
  if (!_errNo)
    return (true);
  else if (_errNo == EBUSY)
    return (false);
  else
    throw (MutexException("pthread_mutex_trylock() failed", _errNo));
}
