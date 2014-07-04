/**
 * \file log.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Log class declaration
 */

#ifndef LOG_HPP
#define LOG_HPP

#include <string>
#include <iostream>
#include <sstream>

#include "unixfs.hpp"

class LogLine
{
public:
    LogLine(const char* file, const char* func, int line, std::ostream& out = std::cout)
    : _out(out)
    {
        _stream << UnixFs::stripPath(file) << ':' << line << ": in '" << func << "': ";
    }

    ~LogLine()
    {
        _stream << std::endl;
        _out << _stream.rdbuf(); // NOTE C++11 garantees thread safety
        _out.flush();
    }

    template <class T>
    LogLine& operator<<(const T& arg)
    {
        _stream << arg;
        return (*this);
    }

private:
    std::stringstream   _stream;
    std::ostream&       _out;
};

#define LOG() LogLine(__FILE__, __FUNCTION__, __LINE__)

#endif // LOG_HPP
