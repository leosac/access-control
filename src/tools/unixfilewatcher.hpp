/**
 * \file unixfilewatcher.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief UnixFileWatcher class declaration
 */

#ifndef UNIXFILEWATCHER_HPP
#define UNIXFILEWATCHER_HPP

#include <string>
#include <thread>
#include <atomic>
#include <map>

class UnixFileWatcher
{
    typedef int UnixFd;
    typedef std::string WatchParams;
    typedef std::map<UnixFd, WatchParams> Watches;

    static const long   DefaultTimeoutMs = 2000;

public:
    explicit UnixFileWatcher();
    ~UnixFileWatcher() = default;

    UnixFileWatcher(const UnixFileWatcher& other) = delete;
    UnixFileWatcher& operator=(const UnixFileWatcher& other) = delete;

public:
    // TODO add event recovery methods
    void        start();
    void        stop();
    void        watchFile(const std::string& path);
    std::size_t size() const;

private:
    void    run();

private:
    std::thread         _thread;
    std::atomic<bool>   _isRunning;
    UnixFd              _inotifyFd;
    Watches             _watches;
};

#endif // UNIXFILEWATCHER_HPP
