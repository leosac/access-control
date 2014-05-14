/**
 * \file event.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief event class for intermodule communications
 */

#ifndef EVENT_HPP
#define EVENT_HPP

#include <string>
#include <chrono>

class Event
{
public:
    enum class LogLevel : unsigned int {
        Error = 0,
        Warning,
        Info,
        Debug
    };
    typedef std::chrono::system_clock::time_point Date;

public:
    explicit Event(const std::string& msg, const std::string& src = std::string(), const std::string& dest = std::string(), LogLevel level = LogLevel::Debug);
    ~Event() = default;

public:
    bool operator<(const Event& other) const;

public:
    static const std::string&   getLogLevelString(Event::LogLevel level);

public:
    std::string         message;
    std::string         source;
    std::string         destination;
    LogLevel            logLevel;
    Date                date;
};

#endif // EVENT_HPP
