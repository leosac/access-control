/**
 * \file journallogger.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief event logger module class
 */

#include "journallogger.hpp"

#include <iostream> // FIXME Debug
#include <ctime> // FIXME Debug

JournalLogger::JournalLogger()
{

}

JournalLogger::~JournalLogger()
{

}

void JournalLogger::sendEvent(const Event& event)
{
    std::cout << std::ctime(&event.date) << "Event: [L=" << event.logLevel << "] " << event.source << ": " << event.message<< std::endl;
}
