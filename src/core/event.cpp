/**
 * \file event.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief event class for intermodule communications
 */

#include "event.hpp"

#include <chrono>

static const int            logLevelCount = 8;
static const std::string    logLevelStrings[logLevelCount + 1] = {
    "EMERGENCY",
    "ALERT",
    "CRITICAL",
    "ERROR",
    "WARNING",
    "NOTICE",
    "INFO",
    "DEBUG",
    "(unknown)"
};

Event::Event(const std::string& msg, const std::string& src, LogLevel level)
:   message(msg),
    source(src),
    logLevel(level),
    date(std::chrono::system_clock::now())
{}

Event::~Event() {}

const std::string& Event::getLogLevelString(int level)
{
    if (logLevelCount && level < logLevelCount)
        return (logLevelStrings[level]);
    else
        return (logLevelStrings[logLevelCount]);
}
