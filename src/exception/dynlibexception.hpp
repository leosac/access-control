/**
 * \file dynlibexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for DynLib related errors
 */

#ifndef DYNLIBEXCEPTION_HPP
#define DYNLIBEXCEPTION_HPP

#include <string>

#include "osacexception.hpp"

class DynLibException : public OSACException
{
public:
    DynLibException(const std::string& message) : OSACException("DynLib::" + message) {};
    virtual ~DynLibException() throw() {};
};

#endif // DYNLIBEXCEPTION_HPP
