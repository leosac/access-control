/**
 * \file gpioexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for Gpio related errors
 */

#ifndef GPIOEXCEPTION_HPP
#define GPIOEXCEPTION_HPP

#include <string>

#include "leosacexception.hpp"

class GpioException : public LEOSACException
{
public:
    GpioException(const std::string& message) : LEOSACException("Gpio::" + message) {}
    virtual ~GpioException() {}
};

#endif // GPIOEXCEPTION_HPP
