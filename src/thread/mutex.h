/**
 * \file Mutex.h
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief Object wrapper around pthread mutexes
 */

#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>

#include "imutex.h"

class Mutex : public IMutex
{
public:
    Mutex(); // NOTE May throw MutexException
    virtual ~Mutex(); // NOTE May throw MutexException

private:
    Mutex(const Mutex& other);
    Mutex& operator=(const Mutex& other);

public:
    void  lock(); // NOTE May throw MutexException
    void  unlock(); // NOTE May throw MutexException
    bool  trylock(); // NOTE May throw MutexException

private:
    pthread_mutex_t   _mutex;
    unsigned          _errNo;
};

#endif // MUTEX_H
