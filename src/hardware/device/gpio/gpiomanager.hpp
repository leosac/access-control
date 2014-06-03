/**
 * \file gpiomanager.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief GPIO device managerclass
 */

#ifndef GPIOMANAGER_HPP
#define GPIOMANAGER_HPP

#include "gpio.hpp"

#include <poll.h>

#include <atomic>
#include <thread>
#include <list>
#include <vector>
#include <map>

#include "igpioobservable.hpp"
#include "igpioprovider.hpp"

class IGPIOListener;

class GPIOManager : public IGPIOObservable, public IGPIOProvider
{
    static const int            DefaultTimeout = 100;
    static const unsigned int   PollBufferSize = 64;

public:
    typedef struct pollfd PollFdSet;
    typedef struct {
        IGPIOListener*  instance;
        int             gpioNo;
        GPIO::EdgeMode  mode;
        unsigned int    fdIdx;
    } ListenerInfo;

public:
    explicit GPIOManager();
    ~GPIOManager() = default;

    GPIOManager(const GPIOManager& other) = delete;
    GPIOManager& operator=(const GPIOManager& other) = delete;

public:
    virtual void    registerListener(IGPIOListener* listener, int gpioNo, GPIO::EdgeMode mode) override; // NOTE call this BEFORE starting to poll
    virtual GPIO*   getGPIO(int gpioNo) override;

public:
    void    startPolling();
    void    stopPolling();
    void    pollLoop();

private:
    void    timeout();
    void    buildFdSet();

private:
    std::atomic<bool>       _isRunning;
    std::thread             _pollThread;
    int                     _pollTimeout;
    std::map<int, GPIO*>    _polledGpio;
    std::list<ListenerInfo> _listeners;
    std::vector<PollFdSet>  _fdset;
};

#endif // GPIOMANAGER_HPP
