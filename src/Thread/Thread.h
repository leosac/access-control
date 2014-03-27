/**
 * \file Thread.h
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief Object wrapper around posix threads
 */

#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

#include "IThread.h"

class Thread : public IThread
{
public:
  Thread();
  virtual ~Thread();

private:
  Thread(const Thread& other);
  Thread& operator=(const Thread& other);

public:
  void	start(IRunnable* runnable); // NOTE May throw ThreadException
  void 	join(); // NOTE May throw ThreadException

private:
  static void*	launch(void* thread);

private:
  pthread_t	_thread;
  unsigned	_errNo;
};

#endif // THREAD_H
