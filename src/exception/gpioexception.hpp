/**
 * \file gpioexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for Gpio related errors
 */

#ifndef GPIOEXCEPTION_HPP
#define GPIOEXCEPTION_HPP

#include <string>

#include "osacexception.hpp"

class GpioException : public OSACException
{
public:
    GpioException(const std::string& message) : OSACException("Gpio::" + message) {};
    virtual ~GpioException() = default;
};

#endif // GPIOEXCEPTION_HPP
