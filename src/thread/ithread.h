/**
 * \file IThread.h
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief Thread interface
 */

#ifndef ITHREAD_H
#define ITHREAD_H

#include "irunnable.h"

class IThread
{
public:
  virtual ~IThread() {}
  virtual void  start(IRunnable* runnable) = 0;
  virtual void  join() = 0;
};

#endif // ITHREAD_H
