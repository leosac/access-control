/**
 * \file thread.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief Object wrapper around posix threads
 */

#include "thread.hpp"
#include "exception/threadexception.hpp"

Thread::Thread() {}

Thread::~Thread() {}

Thread::Thread(const Thread& /*other*/) {}

Thread& Thread::operator=(const Thread& /*other*/)
{
    return (*this);
}

void Thread::start(IRunnable* runnable)
{
    if ((_errNo = pthread_create(&_thread, NULL, &launch, runnable)))
        throw (ThreadException("pthread_create() failed", _errNo));
}

void Thread::join()
{
    if ((_errNo = pthread_join(_thread, NULL)))
        throw (ThreadException("pthread_join() failed", _errNo));
}

void* Thread::launch(void* thread)
{
    static_cast<IRunnable*>(thread)->run();
    return (NULL);
}
