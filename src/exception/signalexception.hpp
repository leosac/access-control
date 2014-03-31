/**
 * \file signalexception.hpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
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
    virtual ~SignalException() throw() {};
};

#endif // SIGNALEXCEPTION_HPP
