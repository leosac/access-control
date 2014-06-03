/**
 * \file moduleexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for modules
 */

#ifndef MODULEEXCEPTION_HPP
#define MODULEEXCEPTION_HPP

#include <string>

#include "osacexception.hpp"

class ModuleException : public OSACException
{
public:
    ModuleException(const std::string& message) : OSACException("Module::" + message) {}
    virtual ~ModuleException() {}
};

#endif // MODULEEXCEPTION_HPP
