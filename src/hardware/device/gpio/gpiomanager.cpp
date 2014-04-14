/**
 * \file gpiomanager.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief GPIO device manager class
 */

#include "gpiomanager.hpp"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include <thread>
#include <iostream>

#include "exception/gpioexception.hpp"
#include "tools/unixsyscall.hpp"

static void launch(GPIOManager* instance)
{
    instance->run();
}

GPIOManager::GPIOManager()
:   _isRunning(true),
    _needFdUpdate(true),
    _pollTimeout(1000) // FIXME Debug
{
    _pollThread = std::thread(&launch, this);
    std::cout << "thread started" << std::endl;
}

GPIOManager::~GPIOManager()
{
    std::cout << ">>> ~GPIOManager()" << std::endl;
    {
        std::lock_guard<std::mutex> lg(_runMutex);
        _isRunning = false;
    }
    // TODO Deblock poll()
    _pollThread.join();
    std::cout << "thread joined" << std::endl;
    std::cout << "<<< ~GPIOManager()" << std::endl;
}

GPIOManager::GPIOManager(const GPIOManager& /*other*/) {}

GPIOManager& GPIOManager::operator=(const GPIOManager& /*other*/)
{
    return (*this);
}

void GPIOManager::registerListener(IGPIOListener* instance, int gpioNo, GPIO::EdgeMode mode)
{
    ListenerInfo    listener;

    listener.instance = instance;
    listener.gpioNo = gpioNo;
    listener.mode = mode;
    listener.fdIdx = 0;

    if (!_gpio[listener.gpioNo])
        reserveGPIO(listener.gpioNo);
    _listeners.push_back(listener);
    rebuildFdSet();
}

void GPIOManager::run()
{
    const unsigned  bufferLen = 32;
    char            buffer[bufferLen];
    int             ret;
    unsigned int    setSize;

    std::cout << "run() started" << std::endl;
    {
        std::lock_guard<std::mutex> lg(_runMutex);
        while (_isRunning)
        {
            _runMutex.unlock();
            setSize = _fdset.size();

            if (_needFdUpdate)
                resetFdSet(setSize);

            if ((ret = ::poll(&_fdset[0], setSize, _pollTimeout)) < 0)
                throw (GpioException(UnixSyscall::getErrorString("poll", errno)));
            if (!ret)
                continue;

            _listenerMutex.lock();
            for (unsigned int i = 0; i < setSize; ++i)
            {
                if (_fdset[i].revents & POLLPRI)
                {
                    if ((ret = ::read(_fdset[i].fd, buffer, bufferLen - 1)) < 0)
                        throw (GpioException(UnixSyscall::getErrorString("read", errno)));
                    else if (ret > 1)
                        buffer[ret - 1] = '\0';
                    if (::lseek(_fdset[i].fd, 0, SEEK_SET) < 0)
                        throw (GpioException(UnixSyscall::getErrorString("lseek", errno)));
                    for (auto listener : _listeners)
                    {
                        if (listener.fdIdx == i)
                            listener.instance->notify(listener.gpioNo);
                    }
                }
            }
            _runMutex.lock();
            _isRunning = false; // FIXME Debug
        }
    }
    std::cout << "run() ended" << std::endl;
}

void GPIOManager::reserveGPIO(int gpioNo)
{
    _gpio[gpioNo] = new GPIO(gpioNo);
}

void GPIOManager::rebuildFdSet()
{
    int i = 0;

    if (_fdset.size() != _listeners.size())
        _fdset.resize(_listeners.size());
    for (auto listener : _listeners)
    {
        _fdset[i].fd = _gpio[listener.gpioNo]->getPollFd();
        _fdset[i].events = POLLPRI;
        ++i;
    }
}

void GPIOManager::resetFdSet(unsigned int size)
{
    for (unsigned int i = 0; i < size; ++i)
        _fdset[i].revents = 0;
}
