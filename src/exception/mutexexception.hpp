/**
 * \file MutexException.hpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief Exception class for Mutex errors
 */

#ifndef MUTEXEXCEPTION_HPP
#define MUTEXEXCEPTION_HPP

#include "syscallexception.hpp"

class MutexException : public SyscallException
{
public:
  MutexException(const std::string& message, unsigned errNo = 0) : SyscallException("Mutex::" + message, errNo) {};
  virtual ~MutexException() throw() {};
};

#endif // MUTEXEXCEPTION_HPP
