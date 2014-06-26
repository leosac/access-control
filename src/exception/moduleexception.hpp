/**
 * \file moduleexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for modules
 */

#ifndef MODULEEXCEPTION_HPP
#define MODULEEXCEPTION_HPP

#include <string>

#include "leosacexception.hpp"

class ModuleException : public LEOSACException
{
public:
    ModuleException(const std::string& message) : LEOSACException("Module::" + message) {}
    virtual ~ModuleException() {}
};

#endif // MODULEEXCEPTION_HPP
