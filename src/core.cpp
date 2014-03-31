/**
 * \file core.cpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief module managing class
 */

#include "core.hpp"
#include "modules/journallogger.hpp"
#include "hardware/hwmanager.hpp"

#include <thread>
#include <iostream>

Core::Core()
:   _isRunning(true),
    _hwManager(0)
{}

Core::~Core() {}

Core::Core(const Core& /*other*/) {}

Core& Core::operator=(const Core& /*other*/)
{
    return (*this);
}

void Core::run()
{
    load();
    dispatchEvent(Event("starting", "Core"));
    _runMutex.lock();
    while (_isRunning)
    {
        _runMutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        dispatchEvent(Event("alive", "Core"));
        _runMutex.lock();

        _isRunning = false;
    }
    _runMutex.unlock();
    dispatchEvent(Event("exiting", "Core"));
    unload();
}

void Core::load()
{
    _hwManager = new HWManager;
    _loggerModules.push_front(new JournalLogger(Event::Debug));
}

void Core::unload()
{
    for (auto logger : _loggerModules)
        delete logger;
    _loggerModules.clear();

    delete _hwManager;
}

void Core::dispatchEvent(const Event& event)
{
    for (auto logger : _loggerModules)
        logger->sendEvent(event);
}
