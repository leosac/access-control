/**
 * \file journallogger.hpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief event logger module class
 */

#ifndef JOURNALLOGGER_HPP
#define JOURNALLOGGER_HPP

#include "iloggermodule.hpp"

class JournalLogger : public ILoggerModule
{
public:
    JournalLogger();
    ~JournalLogger();

public:
    void    sendEvent(const Event& event);
};

#endif // JOURNALLOGGER_HPP
