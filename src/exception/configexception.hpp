/**
 * \file configexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for Config related errors
 */

#ifndef CONFIGEXCEPTION_HPP
#define CONFIGEXCEPTION_HPP

#include <string>

#include "leosacexception.hpp"

class ConfigException : public LEOSACException
{
public:
    ConfigException(const std::string& file, const std::string& message) : LEOSACException("Config::" + file + ": "  + message) {}
    virtual ~ConfigException() {}
};

#endif // CONFIGEXCEPTION_HPP
