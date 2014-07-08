/**
 * \file scriptexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for Script related errors
 */

#ifndef SCRIPTEXCEPTION_HPP
#define SCRIPTEXCEPTION_HPP

#include <string>

#include "leosacexception.hpp"

class ScriptException : public LEOSACException
{
public:
    ScriptException(const std::string& message) : LEOSACException("Script::" + message) {}
    virtual ~ScriptException() {}
};

#endif // SCRIPTEXCEPTION_HPP
