/**
 * \file dynlibexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for DynLib related errors
 */

#ifndef DYNLIBEXCEPTION_HPP
#define DYNLIBEXCEPTION_HPP

#include <string>

#include "leosacexception.hpp"

class DynLibException : public LEOSACException
{
public:
    DynLibException(const std::string& message) : LEOSACException("DynLib::" + message) {}
    virtual ~DynLibException() {}
};

#endif // DYNLIBEXCEPTION_HPP
