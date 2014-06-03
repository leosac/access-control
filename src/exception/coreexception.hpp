/**
 * \file coreexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for Core related errors
 */

#ifndef COREEXCEPTION_HPP
#define COREEXCEPTION_HPP

#include <string>

#include "osacexception.hpp"

class CoreException : public OSACException
{
public:
    CoreException(const std::string& message) : OSACException("Core::" + message) {}
    virtual ~CoreException() {}
};

#endif // COREEXCEPTION_HPP
