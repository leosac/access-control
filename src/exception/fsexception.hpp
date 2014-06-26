/**
 * \file fsexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for filesystem related errors
 */

#ifndef FSEXCEPTION_HPP
#define FSEXCEPTION_HPP

#include <string>

#include "leosacexception.hpp"

class FsException : public LEOSACException
{
public:
    FsException(const std::string& message) : LEOSACException("Fs::" + message) {}
    virtual ~FsException() {}
};

#endif // FSEXCEPTION_HPP
