/**
 * \file rplethauth.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief rpleth compatibility module
 */

#ifndef RPLETHAUTH_HPP
#define RPLETHAUTH_HPP

#include <list>
#include <thread>
#include <mutex>

#include "modules/imodule.hpp"
#include "network/isocket.hpp"

class RplethAuth : public IModule
{
    static const Rezzo::ISocket::Port   DefaultPort = 9559;
    static const long                   DefaultTimeoutMs = 500;
public:
    RplethAuth(Rezzo::ISocket::Port port = DefaultPort, long timeoutMs = DefaultTimeoutMs);
    ~RplethAuth();

public:
    void                sendEvent(const Event& event);
    Type                getType() const;
    const std::string&  getVersionString() const;
    void                run();

private:
    const std::string           _version;
    bool                        _isRunning;
    std::mutex                  _runMutex;
    std::thread                 _networkThread;
    Rezzo::ISocket*             _serverSocket;
    Rezzo::ISocket::Port        _port;
    std::list<Rezzo::ISocket*>  _clients;
    fd_set                      _rSet;
    int                         _fdMax;
    long                        _timeout;
    struct timeval              _timeoutStruct;
};

#endif // RPLETHAUTH_HPP
