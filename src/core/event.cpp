/**
 * \file event.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief event class for intermodule communications
 */

#include "event.hpp"

#include <iostream>

static const unsigned int   logLevelCount = 4;
static const std::string    logLevelStrings[logLevelCount] = {
    "ERROR",
    "WARNING",
    "INFO",
    "DEBUG"
};

Event::Event(const std::string& msg, const std::string& src, const std::string& dest, LogLevel level)
:   message(msg),
    source(src),
    destination(dest),
    logLevel(level),
    date(std::chrono::system_clock::now())
{}

bool Event::operator<(const Event& other) const
{
    return (date < other.date);
}

const std::string& Event::getLogLevelString(Event::LogLevel level)
{
    return (logLevelStrings[static_cast<unsigned int>(level)]); // NOTE Should be safe due to enum class type safety
}
