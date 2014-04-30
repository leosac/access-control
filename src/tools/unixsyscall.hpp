/**
 * \file unixsyscall.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief unix syscall helper functions
 */

#ifndef UNIXSYSCALL_HPP
#define UNIXSYSCALL_HPP

#include <string>

class UnixSyscall
{
    UnixSyscall() = delete;
    
public:
    static std::string getErrorString(const std::string& func, int errNo);
};

#endif // UNIXSYSCALL_HPP
