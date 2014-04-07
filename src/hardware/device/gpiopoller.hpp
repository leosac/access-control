/**
 * \file gpiopoller.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief GPIO helper class handling input polling
 */

#ifndef GPIOPOLLER_HPP
#define GPIOPOLLER_HPP

#include <string>
#include <mutex>
#include <thread>

class GPIOPoller
{
    static const int    PollTimeoutDelayMs = 100;
public:
    GPIOPoller(const std::string& file);
    ~GPIOPoller();

private:
    GPIOPoller(const GPIOPoller& other);
    GPIOPoller& operator=(const GPIOPoller& other);

private:
    static void launch(GPIOPoller* instance);

public:
    void    run();

public:
    void    start();
    void    stop();
    bool    isRunning();

private:
    const std::string   _file;
    std::mutex          _runMutex;
    bool                _isRunning;
    std::thread*        _pollThread;
};

#endif // GPIOPOLLER_HPP
