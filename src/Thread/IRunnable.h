/**
 * \file IRunnable.h
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief interface for threaded classes
 */

#ifndef IRUNNABLE_H
#define IRUNNABLE_H

class IRunnable
{
public:
  virtual ~IRunnable() {}
  virtual void	run() = 0;
};

#endif // IRUNNABLE_H
