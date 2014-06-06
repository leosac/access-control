/**
 * \file journallogger.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief event logger module class
 */

#ifndef JOURNALLOGGER_HPP
#define JOURNALLOGGER_HPP

#include "modules/imodule.hpp"
#include "core/event.hpp"

class JournalLogger : public IModule
{
public:
    explicit JournalLogger(const std::string& name, Event::LogLevel logLevel = Event::LogLevel::Debug);
    ~JournalLogger() = default;

    JournalLogger(const JournalLogger& other) = delete;
    JournalLogger& operator=(const JournalLogger& other) = delete;

public:
    virtual const std::string&  getName() const override;
    virtual ModuleType          getType() const override;
    virtual void                serialize(ptree& node) override;
    virtual void                deserialize(const ptree& node) override;

private:
    const std::string   _name;
    Event::LogLevel     _logLevel;
};

#endif // JOURNALLOGGER_HPP
