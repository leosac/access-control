/**
 * \file journallogger.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief event logger module class
 */

#include "journallogger.hpp"

#include <iostream>
#include <sstream>
#include <ctime>
#include <chrono>

JournalLogger::JournalLogger(int logLevel)
:   _logLevel(logLevel)
{}

JournalLogger::~JournalLogger() {}

void JournalLogger::sendEvent(const Event& event)
{
    std::stringstream   ss;
    std::time_t         now_c = std::chrono::system_clock::to_time_t(event.date);
    struct tm*          l = std::localtime(&now_c);

    if (event.logLevel > _logLevel)
        return;

    ss << l->tm_year + 1900 << '/';
    ss.fill('0');
    ss.width(2);
    ss << l->tm_mon + 1 << '/';
    ss.fill('0');
    ss.width(2);
    ss << l->tm_mday << ' ';
    ss.fill('0');
    ss.width(2);
    ss << l->tm_hour << 'h';
    ss.fill('0');
    ss.width(2);
    ss << l->tm_min << ' ';
    ss.fill('0');
    ss.width(2);
    ss << l->tm_sec << 's';

    std::clog << '[' << ss.str() << ']' << " " << Event::getLogLevelString(event.logLevel) << " ";
    if (!event.source.empty())
        std::clog << event.source << "::";
    std::clog << event.message<< std::endl;
}
