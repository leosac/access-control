/**
 * \file SyscallException.hpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief Exception class with errno string printing for unix syscall error codes
 */

#ifndef SYSCALLEXCEPTION_HPP
#define SYSCALLEXCEPTION_HPP

#include <cstring>

#include "osacexception.hpp"

class SyscallException : public OSACException
{
public:
    SyscallException(const std::string& message, unsigned errNo = 0) : OSACException(message + ((errNo) ? (" (" + std::string(strerror(errNo)) + ")") : (""))) {};
    virtual ~SyscallException() throw() {};
};

#endif // SYSCALLEXCEPTION_HPP
