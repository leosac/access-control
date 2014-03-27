/**
 * \file ThreadException.hpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief Exception class for Thread errors
 */

#ifndef THREADEXCEPTION_HPP
#define THREADEXCEPTION_HPP

#include <string>

#include "syscallexception.hpp"

class ThreadException : public SyscallException
{
public:
    ThreadException(const std::string& message, unsigned errNo = 0) : SyscallException("Thread::" + message, errNo) {};
    virtual ~ThreadException() throw() {};
};

#endif // THREADEXCEPTION_HPP
