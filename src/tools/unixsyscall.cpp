/**
 * \file unixsyscall.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief unix syscall helper functions
 */

#include "unixsyscall.hpp"

#include <cstring>

using namespace Leosac::Tools;

std::string UnixSyscall::getErrorString(const std::string& func, int errNo)
{
    return (func + "() failed (" + std::string(strerror(errNo)) + ")");
}
