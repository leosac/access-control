/**
 * \file configexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for Config related errors
 */

#ifndef CONFIGEXCEPTION_HPP
#define CONFIGEXCEPTION_HPP

#include <string>

#include "osacexception.hpp"

class ConfigException : public OSACException
{
public:
    ConfigException(const std::string& file, const std::string& message) : OSACException("Config::" + file + ": "  + message) {};
    virtual ~ConfigException() noexcept = default;
};

#endif // CONFIGEXCEPTION_HPP
