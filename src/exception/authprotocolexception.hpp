/**
 * \file authprotocolexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for AuthProtocol
 */

#ifndef AUTHPROTOCOLEXCEPTION_HPP
#define AUTHPROTOCOLEXCEPTION_HPP

#include <string>

#include "osacexception.hpp"

class AuthProtocolException : public OSACException
{
public:
    AuthProtocolException(const std::string& message) : OSACException("AuthProtocol::" + message) {}
    virtual ~AuthProtocolException() {}
};

#endif // AUTHPROTOCOLEXCEPTION_HPP
