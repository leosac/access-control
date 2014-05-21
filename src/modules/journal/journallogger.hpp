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
    explicit JournalLogger(const std::string& name, Event::LogLevel logLevel = Event::LogLevel::Debug);
    ~JournalLogger() noexcept = default;

    JournalLogger(const JournalLogger& other) = delete;
    JournalLogger& operator=(const JournalLogger& other) = delete;

public:
    virtual void                notify(const Event& event) override;
    virtual const std::string&  getName() const override;
    virtual ModuleType          getType() const override;
    virtual void                serialize(boost::property_tree::ptree& node) override;
    virtual void                deserialize(boost::property_tree::ptree& node) override;

private:
    const std::string   _name;
    Event::LogLevel     _logLevel;
};

#endif // JOURNALLOGGER_HPP
