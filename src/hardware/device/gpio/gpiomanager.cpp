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

#include "exception/gpioexception.hpp"
#include "igpiolistener.hpp"
#include "tools/unixsyscall.hpp"

static void launch(GPIOManager* instance)
{
    instance->pollLoop();
}

GPIOManager::GPIOManager()
:   _isRunning(false),
    _pollTimeout(DefaultTimeout)
{}

GPIOManager::~GPIOManager()
{}

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

    if (!_polledGpio[gpioNo])
    {
        GPIO*   gpio = new GPIO(gpioNo);

        gpio->setDirection(GPIO::In);
        gpio->setEdgeMode(mode);
        _polledGpio[gpioNo] = gpio;
    }
    _listeners.push_back(listener);
}

void GPIOManager::startPolling()
{
    _isRunning = true;
    _pollThread = std::thread(&launch, this);
}

void GPIOManager::stopPolling()
{
    {
        std::lock_guard<std::mutex> lg(_runMutex);
        _isRunning = false;
    }
    _pollThread.join();
}

void GPIOManager::pollLoop()
{
    char            buffer[PollBufferSize];
    int             ret;
    unsigned int    fdsetSize;

    buildFdSet();
    fdsetSize = _fdset.size();
    {
        std::lock_guard<std::mutex> lg(_runMutex);
        while (_isRunning)
        {
            _runMutex.unlock();
            if ((ret = ::poll(&_fdset[0], fdsetSize, _pollTimeout)) < 0)
                throw (GpioException(UnixSyscall::getErrorString("poll", errno)));
            else if (!ret)
                timeout();
            else
            {
                for (unsigned int i = 0; i < fdsetSize; ++i)
                {
                    if (_fdset[i].revents & POLLPRI)
                    {
                        _fdset[i].revents = 0;
                        if ((ret = ::read(_fdset[i].fd, buffer, PollBufferSize - 1)) < 0)
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
            }
            _runMutex.lock();
        }
    }
}

void GPIOManager::timeout()
{
    for (auto listener : _listeners)
        listener.instance->timeout();
}

void GPIOManager::buildFdSet()
{
    int i = 0;

    _fdset.resize(_listeners.size());
    for (std::list<ListenerInfo>::iterator it = _listeners.begin(); it != _listeners.end(); ++it)
    {
        _fdset[i].fd = _polledGpio[it->gpioNo]->getPollFd();
        _fdset[i].events = POLLPRI;
        _fdset[i].revents = 0;
        it->fdIdx = i;
        ++i;
    }
}

