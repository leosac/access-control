/**
 * \file journallogger.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief event logger module class
 */

#ifndef JOURNALLOGGER_HPP
#define JOURNALLOGGER_HPP

#include "imodule.hpp"

class JournalLogger : public IModule
{
public:
    JournalLogger(int logLevel = Event::Notice);
    ~JournalLogger();

public:
    void    sendEvent(const Event& event);

private:
    int     _logLevel;
};

#endif // JOURNALLOGGER_HPP
