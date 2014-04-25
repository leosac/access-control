/**
 * \file rplethauth.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief rpleth compatibility module
 */

#ifndef RPLETHAUTH_HPP
#define RPLETHAUTH_HPP

#include <list>
#include <thread>
#include <atomic>

#include "modules/iauthmodule.hpp"
#include "network/isocket.hpp"
#include "network/circularbuffer.hpp"

class RplethAuth : public IAuthModule
{
    static const Rezzo::ISocket::Port   DefaultPort = 9559;
    static const long                   DefaultTimeoutMs = 500;
    static const std::size_t            RingBufferSize = 512;

    typedef struct s_client {
        Rezzo::ISocket* socket;
        CircularBuffer  buffer;
        explicit s_client(Rezzo::ISocket* sock) : socket(sock), buffer(RingBufferSize) {}
    } Client;

public:
    RplethAuth(Rezzo::ISocket::Port port = DefaultPort, long timeoutMs = DefaultTimeoutMs);
    ~RplethAuth();

public:
    void                sendAuthRequest(const AuthRequest& request);
    Type                getType() const;
    const std::string&  getVersionString() const;
    void                run();

private:
    void                handleClientMessage(Client& client);

private:
    const std::string       _version;
    std::atomic<bool>       _isRunning;
    std::thread             _networkThread;
    Rezzo::ISocket*         _serverSocket;
    Rezzo::ISocket::Port    _port;
    std::list<Client>       _clients;
    fd_set                  _rSet;
    int                     _fdMax;
    long                    _timeout;
    struct timeval          _timeoutStruct;
    Byte                    _buffer[RingBufferSize];
};

#endif // RPLETHAUTH_HPP
