/**
 * \file gpiomanager.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief GPIO device manager class
 */

#include "gpiomanager.hpp"

extern "C" {
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
}

#include <thread>

#include "exception/gpioexception.hpp"
#include "igpiolistener.hpp"
#include "tools/log.hpp"
#include "tools/unixsyscall.hpp"

static void launch(GPIOManager* instance)
{
    instance->pollLoop();
}

GPIOManager::GPIOManager()
:   _isRunning(false),
    _pollTimeout(DefaultTimeout)
{}

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

GPIO* GPIOManager::getGPIO(int idx)
{
    return (new GPIO(idx));
}

void GPIOManager::startPolling()
{
    _isRunning = true;
    _pollThread = std::thread(&launch, this);
}

void GPIOManager::stopPolling()
{
    _isRunning = false;
    _pollThread.join();
}

void GPIOManager::pollLoop()
{
    char            buffer[PollBufferSize];
    int             ret;
    unsigned int    fdsetSize;

    LOG() << "starting poller";
    buildFdSet();
    fdsetSize = _fdset.size();
    while (_isRunning)
    {
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
                    if (::lseek(_fdset[i].fd, 0, SEEK_SET) < 0)
                        throw (GpioException(UnixSyscall::getErrorString("lseek", errno)));
                    for (auto& listener : _listeners)
                    {
                        if (listener.fdIdx == i)
                            listener.instance->notify(listener.gpioNo);
                    }
                }
            }
        }
    }
}

void GPIOManager::timeout()
{
    for (auto& listener : _listeners)
        listener.instance->timeout();
}

void GPIOManager::buildFdSet()
{
    int i = 0;

    _fdset.resize(_listeners.size());
    for (auto& listener : _listeners)
    {
        _fdset[i].fd = _polledGpio[listener.gpioNo]->getPollFd();
        _fdset[i].events = POLLPRI;
        _fdset[i].revents = 0;
        listener.fdIdx = i;
        ++i;
    }
}

