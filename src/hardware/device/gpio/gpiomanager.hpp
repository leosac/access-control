/**
 * \file gpiomanager.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief GPIO device managerclass
 */

#ifndef GPIOMANAGER_HPP
#define GPIOMANAGER_HPP

#include "gpio.hpp"

#include <poll.h>

#include <mutex>
#include <thread>
#include <list>
#include <vector>
#include <map>

#include "igpioobservable.hpp"

class IGPIOListener;

class GPIOManager : public IGPIOObservable
{
    static const int DefaultTimeout = 500;
public:
    typedef struct pollfd PollFdSet;
    typedef struct {
        IGPIOListener*  instance;
        int             gpioNo;
        GPIO::EdgeMode  mode;
        unsigned int    fdIdx;
    } ListenerInfo;

public:
    GPIOManager();
    ~GPIOManager();

private:
    GPIOManager(const GPIOManager& other);
    GPIOManager& operator=(const GPIOManager& other);

public:
    void    registerListener(IGPIOListener* listener, int gpioNo, GPIO::EdgeMode mode); // NOTE call this before starting to poll
    void    startPolling();
    void    stopPolling();
    void    pollLoop();

private:
    void    buildFdSet();

private:
    bool                    _isRunning;
    std::mutex              _runMutex;
    std::thread             _pollThread;
    int                     _pollTimeout;
    std::map<int, GPIO*>    _polledGpio;
    std::list<ListenerInfo> _listeners;
    std::vector<PollFdSet>  _fdset;
};

#endif // GPIOMANAGER_HPP
