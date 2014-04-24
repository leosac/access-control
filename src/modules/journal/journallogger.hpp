/**
 * \file journallogger.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief event logger module class
 */

#ifndef JOURNALLOGGER_HPP
#define JOURNALLOGGER_HPP

#include "modules/ieventlistenermodule.hpp"

class JournalLogger : public IEventListenerModule
{
public:
    JournalLogger(int logLevel = Event::Debug);
    ~JournalLogger();

public:
    void                sendEvent(const Event& event);
    Type                getType() const;
    const std::string&  getVersionString() const;

private:
    const std::string   _version;
    int                 _logLevel;
};

#endif // JOURNALLOGGER_HPP
