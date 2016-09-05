/*
    Copyright (C) 2014-2016 Islog

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

#pragma once

#include <cassert>
#include <csignal>
#include <iostream>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>

enum LogLevel
{
    EMERG  = spdlog::level::emerg,
    ALERT  = spdlog::level::alert,
    CRIT   = spdlog::level::critical,
    ERROR  = spdlog::level::err,
    WARN   = spdlog::level::warn,
    NOTICE = spdlog::level::notice,
    INFO   = spdlog::level::info,
    DEBUG  = spdlog::level::debug,
};

namespace LogHelper
{
LogLevel log_level_from_string(const std::string &level);

void log(const std::string &log_msg, int /*line*/, const char * /*funcName*/,
         const char * /*fileName*/, LogLevel level);
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
#define BUILD_STR(param)                                                            \
    [&](void) {                                                                     \
        std::stringstream logger_macro_ss__;                                        \
        logger_macro_ss__ << param;                                                 \
        return logger_macro_ss__.str();                                             \
    }()


/**
* See "Internal macros documentation"
*/
#define DEBUG_0(msg)                                                                \
    LogHelper::log(BUILD_STR(msg), __LINE__, FUNCTION_NAME_MACRO, __FILE__,         \
                   LogLevel::DEBUG)

/**
* See "Internal macros documentation"
*/
#define DEBUG_1(msg, loggers)                                                       \
    LogHelper::log(BUILD_STR(msg), __LINE__, FUNCTION_NAME_MACRO, __FILE__,         \
                   LogLevel::DEBUG, loggers)

/**
* See "Internal macros documentation"
*/
#define DEBUG_X(trash, msg, loggers, targetMacro, ...) targetMacro

/**
* Debug macro.
* Issue a log message with DEBUG level
*/
#define DEBUG(...)                                                                  \
    DEBUG_X(, ##__VA_ARGS__, DEBUG_1(__VA_ARGS__), DEBUG_0(__VA_ARGS__),            \
            DEBUG_NO_PARAM(__VA_ARGS__), )


/**
* See "Internal macros documentation"
*/
#define INFO_0(msg)                                                                 \
    LogHelper::log(BUILD_STR(msg), __LINE__, FUNCTION_NAME_MACRO, __FILE__,         \
                   LogLevel::INFO)

/**
* See "Internal macros documentation"
*/
#define INFO_1(msg, loggers)                                                        \
    LogHelper::log(BUILD_STR(msg), __LINE__, FUNCTION_NAME_MACRO, __FILE__,         \
                   LogLevel::INFO, loggers)

/**
* See "Internal macros documentation"
*/
#define INFO_X(trash, msg, loggers, targetMacro, ...) targetMacro

/**
* Information macro.
* Issue a log message with INFO level.
*/
#define INFO(...)                                                                   \
    INFO_X(, ##__VA_ARGS__, INFO_1(__VA_ARGS__), INFO_0(__VA_ARGS__),               \
           INFO_NO_PARAM(__VA_ARGS__), )


/**
* See "Internal macros documentation"
*/
#define NOTICE_0(msg)                                                               \
    LogHelper::log(BUILD_STR(msg), __LINE__, FUNCTION_NAME_MACRO, __FILE__,         \
                   LogLevel::NOTICE)

/**
* See "Internal macros documentation"
*/
#define NOTICE_1(msg, loggers)                                                      \
    LogHelper::log(BUILD_STR(msg), __LINE__, FUNCTION_NAME_MACRO, __FILE__,         \
                   LogLevel::NOTICE, loggers)

/**
* See "Internal macros documentation"
*/
#define NOTICE_X(trash, msg, loggers, targetMacro, ...) targetMacro

/**
* Notice macro.
* Issue a log message with NOTICE level.
*/
#define NOTICE(...)                                                                 \
    NOTICE_X(, ##__VA_ARGS__, NOTICE_1(__VA_ARGS__), NOTICE_0(__VA_ARGS__),         \
             NOTICE_NO_PARAM(__VA_ARGS__), )


/**
* See "Internal macros documentation"
*/
#define WARN_0(msg)                                                                 \
    LogHelper::log(BUILD_STR(msg), __LINE__, FUNCTION_NAME_MACRO, __FILE__,         \
                   LogLevel::WARN)

/**
* See "Internal macros documentation"
*/
#define WARN_1(msg, loggers)                                                        \
    LogHelper::log(BUILD_STR(msg), __LINE__, FUNCTION_NAME_MACRO, __FILE__,         \
                   LogLevel::WARN)

/**
* See "Internal macros documentation"
*/
#define WARN_X(trash, msg, loggers, targetMacro, ...) targetMacro

/**
* Warning macro.
* Issue a log message with WARN level.
*/
#define WARN(...)                                                                   \
    WARN_X(, ##__VA_ARGS__, WARN_1(__VA_ARGS__), WARN_0(__VA_ARGS__),               \
           WARN_NO_PARAM(__VA_ARGS__), )


/**
* See "Internal macros documentation"
*/
#define ERROR_0(msg)                                                                \
    LogHelper::log(BUILD_STR(msg), __LINE__, FUNCTION_NAME_MACRO, __FILE__,         \
                   LogLevel::ERROR)

/**
* See "Internal macros documentation"
*/
#define ERROR_1(msg, loggers)                                                       \
    LogHelper::log(BUILD_STR(msg), __LINE__, FUNCTION_NAME_MACRO, __FILE__,         \
                   LogLevel::ERROR)

/**
* See "Internal macros documentation"
*/
#define ERROR_X(trash, msg, loggers, targetMacro, ...) targetMacro

/**
* Error macro.
* Issue a log message with ERROR level
*/
#define ERROR(...)                                                                  \
    ERROR_X(, ##__VA_ARGS__, ERROR_1(__VA_ARGS__), ERROR_0(__VA_ARGS__),            \
            ERROR_NO_PARAM(__VA_ARGS__), )


#ifdef NDEBUG
#define ASSERT_LOG(cond, msg)                                                       \
    do                                                                              \
    {                                                                               \
        (void)sizeof(cond);                                                         \
    } while (0)
#else
#define ASSERT_LOG(cond, msg)                                                       \
    do                                                                              \
    {                                                                               \
        if (!(cond))                                                                \
        {                                                                           \
            ERROR(msg);                                                             \
            ERROR("Assertion failed. Aborting.");                                   \
            raise(SIGABRT);                                                         \
        }                                                                           \
    } while (0)
#endif
