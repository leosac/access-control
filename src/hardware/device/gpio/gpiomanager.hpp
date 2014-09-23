/**
 * \file gpiomanager.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief GPIO device manager class
 */

#ifndef GPIOMANAGER_HPP
#define GPIOMANAGER_HPP

#include "gpio.hpp"

extern "C" {
#include <poll.h>
}

#include <atomic>
#include <thread>
#include <list>
#include <vector>
#include <map>
#include <mutex>

#include "igpioprovider.hpp"

class IGPIOListener;

/**
* Poll on the GPIO (managed through sysfs) and notify its listener when interruption are received on any GPIO.
*/
class GPIOManager : public IGPIOProvider
{
    static const int            DefaultTimeout = 100;
    static const unsigned int   PollBufferSize = 64;

public:
    using GpioAliases = std::map<int, std::string>;
    using PollFdSet = struct pollfd;
    struct ListenerInfo {
        IGPIOListener*  instance;
        int             gpioNo;
        unsigned int    fdIdx;
    };

public:
    explicit GPIOManager();
    ~GPIOManager();

    GPIOManager(const GPIOManager& other) = delete;
    GPIOManager& operator=(const GPIOManager& other) = delete;

public:
    virtual GPIO*   getGPIO(int gpioNo) override;
    virtual void    registerListener(IGPIOListener* listener, GPIO* gpio) override;
    virtual void    unregisterListener(IGPIOListener* listener, GPIO* gpio) override;

public:
    const GpioAliases&  getGpioAliases() const;
    void                setGpioAlias(int gpioNo, const std::string& alias);

public:
    void    startPolling();
    void    stopPolling();

private:
    void    pollLoop();
    GPIO*   instanciateGpio(int gpioNo);
    void    timeout();
    void    buildFdSet();

private:
    std::atomic<bool>       _isRunning;
    std::mutex              _listenerMutex;
    std::thread             _pollThread;
    int                     _pollTimeout;
    std::map<int, GPIO*>    _Gpios;
    GpioAliases             _gpioAliases;
    std::list<ListenerInfo> _listeners;
    std::vector<PollFdSet>  _fdset;
};

#endif // GPIOMANAGER_HPP
