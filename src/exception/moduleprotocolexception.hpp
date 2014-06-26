/**
 * \file moduleprotocolexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for ModuleProtocol
 */

#ifndef MODULEPROTOCOLEXCEPTION_HPP
#define MODULEPROTOCOLEXCEPTION_HPP

#include <string>

#include "leosacexception.hpp"

class ModuleProtocolException : public LEOSACException
{
public:
    ModuleProtocolException(const std::string& message) : LEOSACException("ModuleProtocol::" + message) {}
    virtual ~ModuleProtocolException() {}
};

#endif // MODULEPROTOCOLEXCEPTION_HPP
