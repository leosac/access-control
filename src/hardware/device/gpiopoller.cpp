/**
 * \file gpiopoller.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief GPIO helper class handling input polling
 */

#include "gpiopoller.hpp"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include "exception/deviceexception.hpp"
#include "tools/unixsyscall.hpp"

const int GPIOPoller::PollTimeoutDelayMs;

GPIOPoller::GPIOPoller(const std::string& file)
:   _file(file),
    _isRunning(false),
    _pollThread(nullptr)
{}

GPIOPoller::~GPIOPoller()
{
    if (isRunning())
        stop();
}

GPIOPoller::GPIOPoller(const GPIOPoller& /*other*/) {}

GPIOPoller& GPIOPoller::operator=(const GPIOPoller& /*other*/)
{
    return (*this);
}

void GPIOPoller::launch(GPIOPoller* instance)
{
    instance->run();
}

void GPIOPoller::start()
{
    if (_pollThread)
        return;
    {
        std::lock_guard<std::mutex> lg(_runMutex);
        _isRunning = true;
    }
    _pollThread = new std::thread(&launch, this);
}

void GPIOPoller::stop()
{
    if (!_pollThread)
        return;
    {
        std::lock_guard<std::mutex> lg(_runMutex);
        _isRunning = false;
    }
    _pollThread->join();
    delete _pollThread;
    _pollThread = nullptr;
}

bool GPIOPoller::isRunning()
{
    std::lock_guard<std::mutex> lg(_runMutex);
    return (_isRunning);
}

void GPIOPoller::run()
{
    struct pollfd   fdset;
    int             gpioFd;
    const unsigned  bufferLen = 32;
    char            buffer[bufferLen];
    int             ret;

    if ((gpioFd = ::open(_file.c_str(), O_RDONLY | O_NONBLOCK)) == -1)
        throw (DeviceException(UnixSyscall::getErrorString("open", errno)));

    _runMutex.lock();
    while (_isRunning)
    {
        _runMutex.unlock();
        ::memset(&fdset, 0, sizeof(fdset));
        fdset.fd = gpioFd;
        fdset.events = POLLPRI;
        if ((ret = ::poll(&fdset, 1, PollTimeoutDelayMs)) == -1)
            throw (DeviceException(UnixSyscall::getErrorString("poll", errno)));
        if (!ret)
            continue; // poll() timed out
        if (fdset.revents & POLLPRI)
        {
            if ((ret = ::read(fdset.fd, buffer, bufferLen - 1)) == -1)
                throw (DeviceException(UnixSyscall::getErrorString("read", errno)));
            if (ret > 1)
                buffer[ret - 1] = '\0';
            if (::lseek(fdset.fd, 0, SEEK_SET) == -1)
                throw (DeviceException(UnixSyscall::getErrorString("lseek", errno)));
            // TODO CALLBACK
        }
        _runMutex.lock();
    }
    _runMutex.unlock();
    if (::close(gpioFd) == -1)
        throw (DeviceException(UnixSyscall::getErrorString("close", errno)));
}
