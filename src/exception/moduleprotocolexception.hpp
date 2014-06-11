/**
 * \file moduleprotocolexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for ModuleProtocol
 */

#ifndef MODULEPROTOCOLEXCEPTION_HPP
#define MODULEPROTOCOLEXCEPTION_HPP

#include <string>

#include "osacexception.hpp"

class ModuleProtocolException : public OSACException
{
public:
    ModuleProtocolException(const std::string& message) : OSACException("ModuleProtocol::" + message) {}
    virtual ~ModuleProtocolException() {}
};

#endif // MODULEPROTOCOLEXCEPTION_HPP
