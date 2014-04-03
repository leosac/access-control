/**
 * \file unixsyscall.hpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief unix syscall helper functions
 */

#ifndef UNIXSYSCALL_HPP
#define UNIXSYSCALL_HPP

#include <string>

class UnixSyscall
{
    UnixSyscall();
public:
    static std::string getErrorString(const std::string& func, int errNo);
};

#endif // UNIXSYSCALL_HPP
