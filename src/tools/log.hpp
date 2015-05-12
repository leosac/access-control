/*
    Copyright (C) 2014-2015 Islog

    This file is part of Leosac.

    Leosac is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/**
* \file log.hpp
* \author Thibault Schueller <ryp.sqrt@gmail.com>
* \brief Log class declaration
*/

#ifndef LOG_HPP
#define LOG_HPP

#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include "unixfs.hpp"
#include "spdlog/spdlog.h"

enum LogLevel
{
    EMERG = spdlog::level::EMERG,
    ALERT = spdlog::level::ALERT,
    CRIT = spdlog::level::CRITICAL,
    ERROR = spdlog::level::ERR,
    WARN = spdlog::level::WARN,
    NOTICE = spdlog::level::NOTICE,
    INFO = spdlog::level::INFO,
    DEBUG = spdlog::level::DEBUG,
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

    static void log(const std::string &log_msg,
            int /*line*/,
            const char */*funcName*/,
            const char */*fileName*/,
            LogLevel level)
    {
        auto console = spdlog::get("console");
        auto syslog = spdlog::get("syslog");
        if (!console && !syslog)
        {
            std::cerr << "Logger not set-up yet ! Will display log message as is." << std::endl;
            std::cerr << log_msg << std::endl;
            return;
        }

        switch (level)
        {
            case LogLevel::DEBUG:
                if (console)
                    console->debug(log_msg);
                if (syslog)
                    syslog->debug(log_msg);
                break;
            case LogLevel::INFO:
                if (console)
                    console->info(log_msg);
                if (syslog)
                    syslog->info(log_msg);
                break;
            case LogLevel::NOTICE:
                if (console)
                    console->notice(log_msg);
                if (syslog)
                    syslog->notice(log_msg);
                break;
            case LogLevel::WARN:
                if (console)
                    console->warn(log_msg);
                if (syslog)
                    syslog->warn(log_msg);
                break;
            case LogLevel::ERROR:
                if (console)
                    console->error(log_msg);
                if (syslog)
                    syslog->error(log_msg);
                break;
            case LogLevel::CRIT:
                if (console)
                    console->critical(log_msg);
                if (syslog)
                    syslog->critical(log_msg);
                break;
            case LogLevel::ALERT:
                if (console)
                    console->alert(log_msg);
                if (syslog)
                    syslog->alert(log_msg);
                break;
            case LogLevel::EMERG:
                if (console)
                    console->emerg(log_msg);
                if (syslog)
                    syslog->emerg(log_msg);
                break;
        }
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

#define ASSERT_LOG(cond, msg)       \
do {                                \
  if (!(cond)) {                    \
    ERROR(msg);                     \
    assert(0);                      \
    exit(-1);                       \
  }                                 \
}                                   \
while (0)

#endif // LOG_HPP
