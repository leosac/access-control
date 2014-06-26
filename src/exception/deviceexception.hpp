/**
 * \file deviceexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for Device related errors
 */

#ifndef DEVICEEXCEPTION_HPP
#define DEVICEEXCEPTION_HPP

#include <string>

#include "leosacexception.hpp"

class DeviceException : public LEOSACException
{
public:
    DeviceException(const std::string& message) : LEOSACException("Device::" + message) {}
    virtual ~DeviceException() {}
};

#endif // DEVICEEXCEPTION_HPP
