/**
 * \file journallogger.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief event logger module class
 */

#ifndef JOURNALLOGGER_HPP
#define JOURNALLOGGER_HPP

#include "modules/imodule.hpp"

class JournalLogger : public IModule
{
public:
    explicit JournalLogger(Event::LogLevel logLevel = Event::LogLevel::Debug);
    ~JournalLogger() = default;

    JournalLogger(const JournalLogger& other) = delete;
    JournalLogger& operator=(const JournalLogger& other) = delete;

public:
    void                notify(const Event& event);
    ModuleType          getType() const;
    const std::string&  getVersionString() const;

private:
    const std::string   _version;
    Event::LogLevel     _logLevel;
};

#endif // JOURNALLOGGER_HPP
