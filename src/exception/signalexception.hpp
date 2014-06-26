/**
 * \file signalexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for Signal related errors
 */

#ifndef SIGNALEXCEPTION_HPP
#define SIGNALEXCEPTION_HPP

#include <string>

#include "leosacexception.hpp"

class SignalException : public LEOSACException
{
public:
    SignalException(const std::string& message) : LEOSACException("Signal::" + message) {}
    virtual ~SignalException() {}
};

#endif // SIGNALEXCEPTION_HPP
