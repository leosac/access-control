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
#include "tools/unixsyscall.hpp"
#include "tools/log.hpp"
#include <tools/leosac.hpp>

GPIOManager::GPIOManager()
:   _isRunning(false),
    _pollTimeout(DefaultTimeout)
{}

GPIOManager::~GPIOManager()
{
    for (auto gpio : _polledGpio)
        delete gpio.second;
    for (auto gpio : _reservedGpio)
        delete gpio.second;
}

void GPIOManager::registerListener(IGPIOListener* instance, int gpioNo)
{
    ListenerInfo    listener;

    listener.instance = instance;
    listener.gpioNo = gpioNo;
    listener.fdIdx = 0;

    if (!_polledGpio.count(gpioNo))
    {
        GPIO*   gpio = instanciateGpio(gpioNo);

        gpio->setDirection(GPIO::Direction::In);
        if (Leosac::Platform == Leosac::PlatformType::RaspberryPi)
            gpio->setEdgeMode(GPIO::EdgeMode::Rising);
        _polledGpio[gpioNo] = gpio;
    }
    _listeners.push_back(listener);
}

GPIO* GPIOManager::getGPIO(int idx)
{
    // TODO Prevent conflict between reserved and polled

    if (_reservedGpio.count(idx) > 0)
        return (_reservedGpio.at(idx));
    else
    {
        GPIO*   gpio = instanciateGpio(idx);
        _reservedGpio[idx] = gpio;
        return (gpio);
    }
}

const GPIOManager::GpioAliases& GPIOManager::getGpioAliases() const
{
    return (_gpioAliases);
}

void GPIOManager::setGpioAlias(int gpioNo, const std::string& alias)
{
    _gpioAliases[gpioNo] = alias;
    LOG() << "Gpio " << gpioNo << " alias is now " << alias;
}

void GPIOManager::startPolling()
{
    _isRunning = true;
    _pollThread = std::thread([this] () { pollLoop(); } );
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
        {
            if (errno != EINTR)
                throw (GpioException(UnixSyscall::getErrorString("poll", errno)));
        }
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

GPIO* GPIOManager::instanciateGpio(int gpioNo)
{
    if (_gpioAliases.count(gpioNo) > 0)
        return (new GPIO(gpioNo, _gpioAliases.at(gpioNo)));
    else
        return (new GPIO(gpioNo, "gpio" + std::to_string(gpioNo)));
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

