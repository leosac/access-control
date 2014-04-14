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

#include "igpiolistener.hpp"
#include "igpioobservable.hpp"

class GPIOManager : public IGPIOObservable
{
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
    void    registerListener(IGPIOListener* listener, int gpioNo, GPIO::EdgeMode mode);
    void    run();

private:
    void    reserveGPIO(int gpioNo); // NOTE may throw GPIOException
    void    rebuildFdSet();
    void    resetFdSet(unsigned int size);

private:
    bool                    _isRunning;
    std::mutex              _runMutex;
    bool                    _needFdUpdate;
    std::thread             _pollThread;
    int                     _pollTimeout;
    std::mutex              _listenerMutex;
    std::mutex              _updateMutex;
    std::map<int, GPIO*>    _gpio;
    std::list<ListenerInfo> _listeners;
    std::vector<PollFdSet>  _fdset;
};

#endif // GPIOMANAGER_HPP
