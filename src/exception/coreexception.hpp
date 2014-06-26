/**
 * \file coreexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for Core related errors
 */

#ifndef COREEXCEPTION_HPP
#define COREEXCEPTION_HPP

#include <string>

#include "leosacexception.hpp"

class CoreException : public LEOSACException
{
public:
    CoreException(const std::string& message) : LEOSACException("Core::" + message) {}
    virtual ~CoreException() {}
};

#endif // COREEXCEPTION_HPP
