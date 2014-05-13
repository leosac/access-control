/**
 * \file osacexception.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Exception class for OSAC Project related errors
 */

#ifndef OSACEXCEPTION_HPP
#define OSACEXCEPTION_HPP

#include <exception>
#include <string>

class OSACException : public std::exception
{
protected:
    explicit OSACException(const std::string& message) : _message(message) {};

public:
    virtual ~OSACException() = default;
    virtual const char* what() const noexcept final { return (_message.c_str()); }

private:
    const std::string   _message;
};

#endif // OSACEXCEPTION_HPP
