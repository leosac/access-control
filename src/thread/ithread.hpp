/**
 * \file ithread.hpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief Thread interface
 */

#ifndef ITHREAD_HPP
#define ITHREAD_HPP

#include "irunnable.hpp"

class IThread
{
public:
    virtual ~IThread() {}
    virtual void    start(IRunnable* runnable) = 0;
    virtual void    join() = 0;
};

#endif // ITHREAD_HPP
