/**
 * \file fsexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for filesystem related errors
 */

#ifndef FSEXCEPTION_HPP
#define FSEXCEPTION_HPP

#include <string>

#include "osacexception.hpp"

class FsException : public OSACException
{
public:
    FsException(const std::string& message) : OSACException("Fs::" + message) {};
    virtual ~FsException() {}
};

#endif // FSEXCEPTION_HPP
