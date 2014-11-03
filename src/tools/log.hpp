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
#include <zmqpp/zmqpp.hpp>

/**
* This is declared here so source file including
* log.gpp will have access to the socket.
* It is defined in kernel.cpp
*/
extern thread_local zmqpp::socket *tl_log_socket;


#include <string>
#include <sstream>
#include <syslog.h>

enum class LogLevel
{
    EMERG = LOG_EMERG,
    ALERT = LOG_ALERT,
    CRIT = LOG_CRIT,
    ERROR = LOG_ERR,
    WARN = LOG_WARNING,
    NOTICE = LOG_NOTICE,
    INFO = LOG_INFO,
    DEBUG = LOG_DEBUG,
};

struct LogHelper
{
    static std::string log_level_to_string(LogLevel level)
    {
        switch (level)
        {
            case LogLevel::DEBUG:
                return "DEBUG";
            case LogLevel::INFO:
                return "INFO";
            case LogLevel::NOTICE:
                return "NOTICE";
            case LogLevel::WARN:
                return "WARNING";
            case LogLevel::ERROR:
                return "ERROR";
            case LogLevel::CRIT:
                return "CRITICAL";
            case LogLevel::ALERT:
                return "ALERT";
            case LogLevel::EMERG:
                return "EMERGENCY";
        }
        return "UNKNOWN";
    }

    static LogLevel log_level_from_string(const std::string &level)
    {
        if (level == "DEBUG")
            return LogLevel::DEBUG;
        else if (level == "INFO")
            return LogLevel::INFO;
        else if (level == "NOTICE")
            return LogLevel::NOTICE;
        else if (level == "WARNING")
            return LogLevel::WARN;
        else if (level == "ERROR")
            return LogLevel::ERROR;
        else if (level == "ALERT")
            return LogLevel::ALERT;
        else if (level == "EMERGENCY")
            return LogLevel::EMERG;
        throw std::runtime_error("Invalid log level: " + level);
    }

    static void log(const std::string &log_msg, int line, const char *funcName,
            const char *fileName, LogLevel level)
    {
        zmqpp::message msg;
        msg << fileName;
        msg << funcName;
        msg << line;
        msg << static_cast<int>(level);
        msg << log_msg;

        assert(tl_log_socket);
        tl_log_socket->send(msg);
    }

};

/**
* @def FUNCTION_NAME_MACRO
* Macro used to get the name of the function that called the logging macro.
* GNU compilers provide a nice macro the get a more detailed name of a function.
* Otherwise we fallback to a more widespread macro.
*/
#ifdef __GNUC__
#define FUNCTION_NAME_MACRO __PRETTY_FUNCTION__
#else
#define FUNCTION_NAME_MACRO __FUNCTION__
#endif

/**
* Internal macro.
* It is used to create an anonymous lambda to allow the logging macros parameter
* to behave like a stringstream.
*/
#define BUILD(param) [&] (void) {std::stringstream logger_macro_ss__; logger_macro_ss__ << param; \
 return logger_macro_ss__.str();}()
/**
* See "Internal macros documentation"
*/
#define DEBUG_0(msg) LogHelper::log(BUILD(msg), __LINE__, FUNCTION_NAME_MACRO, \
__FILE__, LogLevel::DEBUG)
/**
* See "Internal macros documentation"
*/
#define DEBUG_1(msg, loggers) LogHelper::log(BUILD(msg), __LINE__, FUNCTION_NAME_MACRO, \
__FILE__, LogLevel::DEBUG, loggers)
/**
* See "Internal macros documentation"
*/
#define DEBUG_X(trash, msg, loggers, targetMacro, ...) targetMacro
/**
* Debug macro.
* Issue a log message with DEBUG level
*/
#define DEBUG(...) DEBUG_X(, ##__VA_ARGS__, \
DEBUG_1(__VA_ARGS__), \
DEBUG_0(__VA_ARGS__), \
DEBUG_NO_PARAM(__VA_ARGS__), \
)
/**
* See "Internal macros documentation"
*/
#define INFO_0(msg) LogHelper::log(BUILD(msg), __LINE__, FUNCTION_NAME_MACRO, \
__FILE__, LogLevel::INFO)
/**
* See "Internal macros documentation"
*/
#define INFO_1(msg, loggers) LogHelper::log(BUILD(msg), __LINE__, FUNCTION_NAME_MACRO, \
__FILE__, LogLevel::INFO, loggers)
/**
* See "Internal macros documentation"
*/
#define INFO_X(trash, msg, loggers, targetMacro, ...) targetMacro
/**
* Information macro.
* Issue a log message with INFO level.
*/
#define INFO(...) INFO_X(, ##__VA_ARGS__, \
INFO_1(__VA_ARGS__), \
INFO_0(__VA_ARGS__), \
INFO_NO_PARAM(__VA_ARGS__), \
)

/**
* See "Internal macros documentation"
*/
#define NOTICE_0(msg) LogHelper::log(BUILD(msg), __LINE__, FUNCTION_NAME_MACRO, \
__FILE__, LogLevel::NOTICE)
/**
* See "Internal macros documentation"
*/
#define NOTICE_1(msg, loggers) LogHelper::log(BUILD(msg), __LINE__, FUNCTION_NAME_MACRO, \
__FILE__, LogLevel::NOTICE, loggers)
/**
* See "Internal macros documentation"
*/
#define NOTICE_X(trash, msg, loggers, targetMacro, ...) targetMacro
/**
* Notice macro.
* Issue a log message with NOTICE level.
*/
#define NOTICE(...) NOTICE_X(, ##__VA_ARGS__, \
NOTICE_1(__VA_ARGS__), \
NOTICE_0(__VA_ARGS__), \
NOTICE_NO_PARAM(__VA_ARGS__), \
)

/**
* See "Internal macros documentation"
*/
#define WARN_0(msg) LogHelper::log(BUILD(msg), __LINE__, FUNCTION_NAME_MACRO, \
__FILE__, LogLevel::WARN)
/**
* See "Internal macros documentation"
*/
#define WARN_1(msg, loggers) LogHelper::log(BUILD(msg), __LINE__, FUNCTION_NAME_MACRO, \
__FILE__, LogLevel::WARN)
/**
* See "Internal macros documentation"
*/
#define WARN_X(trash, msg, loggers, targetMacro, ...) targetMacro
/**
* Warning macro.
* Issue a log message with WARN level.
*/
#define WARN(...) WARN_X(, ##__VA_ARGS__, \
WARN_1(__VA_ARGS__), \
WARN_0(__VA_ARGS__), \
WARN_NO_PARAM(__VA_ARGS__), \
)
/**
* See "Internal macros documentation"
*/
#define ERROR_0(msg) LogHelper::log(BUILD(msg), __LINE__, FUNCTION_NAME_MACRO, \
__FILE__, LogLevel::ERROR)
/**
* See "Internal macros documentation"
*/
#define ERROR_1(msg, loggers) LogHelper::log(BUILD(msg), __LINE__, FUNCTION_NAME_MACRO, \
__FILE__, LogLevel::ERROR)
/**
* See "Internal macros documentation"
*/
#define ERROR_X(trash, msg, loggers, targetMacro, ...) targetMacro
/**
* Error macro.
* Issue a log message with ERROR level
*/
#define ERROR(...) ERROR_X(, ##__VA_ARGS__, \
ERROR_1(__VA_ARGS__), \
ERROR_0(__VA_ARGS__), \
ERROR_NO_PARAM(__VA_ARGS__), \
)

class LogLine
{
public:
    LogLine(const char *file, const char *func, int line, std::ostream &out = std::cout)
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

    template<class T>
    LogLine &operator<<(const T &arg)
    {
        _stream << arg;
        return (*this);
    }

private:
    std::stringstream _stream;
    std::ostream &_out;
};

#define LOG() LogLine(__FILE__, __FUNCTION__, __LINE__)

#endif // LOG_HPP
