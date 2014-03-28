/**
 * \file core.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief module managing class
 */

#include "core.hpp"
#include "modules/journallogger.hpp"

#include <thread>
#include <iostream>

Core::Core()
: _isRunning(true)
{}

Core::~Core() {}

Core::Core(const Core& /*other*/) {}

Core& Core::operator=(const Core& /*other*/)
{
    return (*this);
}

void Core::run()
{
    _loggerModules.push_front(new JournalLogger);
    _loggerModules.push_front(new JournalLogger);

    Event ev;

    ev.logLevel = Event::Debug;
    ev.message = std::string("salut");
    ev.source = std::string("Core");
    dispatchEvent(ev);

    _runMutex.lock();
    while (_isRunning)
    {
        _runMutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        _runMutex.lock();
    }
    _runMutex.unlock();

    std::cout << "Core exited." << std::endl;
}

void Core::dispatchEvent(const Event& event)
{
    for (auto logger : _loggerModules)
        logger->sendEvent(event);
}
