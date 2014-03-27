/**
 * \file IRunnable.h
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief interface for threaded classes
 */

#ifndef IRUNNABLE_HPP
#define IRUNNABLE_HPP

class IRunnable
{
public:
    virtual ~IRunnable() {}
    virtual void    run() = 0;
};

#endif // IRUNNABLE_HPP
