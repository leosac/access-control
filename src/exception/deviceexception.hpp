/**
 * \file deviceexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for Device related errors
 */

#ifndef DEVICEEXCEPTION_HPP
#define DEVICEEXCEPTION_HPP

#include <string>

#include "osacexception.hpp"

class DeviceException : public OSACException
{
public:
    DeviceException(const std::string& message) : OSACException("Device::" + message) {};
    virtual ~DeviceException() noexcept = default;
};

#endif // DEVICEEXCEPTION_HPP
