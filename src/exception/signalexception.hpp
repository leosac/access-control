/**
 * \file signalexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for Signal related errors
 */

#ifndef SIGNALEXCEPTION_HPP
#define SIGNALEXCEPTION_HPP

#include <string>

#include "osacexception.hpp"

class SignalException : public OSACException
{
public:
    SignalException(const std::string& message) : OSACException("Signal::" + message) {};
    virtual ~SignalException() throw() = default;
};

#endif // SIGNALEXCEPTION_HPP
