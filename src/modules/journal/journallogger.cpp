/**
 * \file journallogger.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief event logger module class
 */

#include "journallogger.hpp"

#include <iostream>
#include <sstream>
#include <ctime>
#include <chrono>

JournalLogger::JournalLogger(const std::string& name, Event::LogLevel logLevel)
:   _name(name),
    _logLevel(logLevel)
{}

void JournalLogger::notify(const Event& event)
{
    std::ostringstream  oss;
    std::time_t         now_c = std::chrono::system_clock::to_time_t(event.date);
    struct tm*          l = std::localtime(&now_c);

    if (event.logLevel > _logLevel)
        return;

    oss << l->tm_year + 1900 << '/';
    oss.fill('0');
    oss.width(2);
    oss << l->tm_mon + 1 << '/';
    oss.fill('0');
    oss.width(2);
    oss << l->tm_mday << ' ';
    oss.fill('0');
    oss.width(2);
    oss << l->tm_hour << 'h';
    oss.fill('0');
    oss.width(2);
    oss << l->tm_min << ' ';
    oss.fill('0');
    oss.width(2);
    oss << l->tm_sec << 's';

    std::clog << '[' << oss.str() << ']' << " " << Event::getLogLevelString(event.logLevel) << " ";
    if (!event.source.empty())
        std::clog << event.source << " said ";
    std::clog << '\"' << event.message << '\"' << std::endl;
}

const std::string& JournalLogger::getName() const
{
    return (_name);
}

IModule::ModuleType JournalLogger::getType() const
{
    return (ModuleType::Logger);
}

void JournalLogger::serialize(boost::property_tree::ptree& node)
{
    static_cast<void>(node);
}

void JournalLogger::deserialize(const boost::property_tree::ptree& node)
{
    static_cast<void>(node);
}
