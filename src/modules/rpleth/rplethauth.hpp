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
#include <queue>
#include <mutex>

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
    typedef std::vector<Byte> CardId;

public:
    explicit RplethAuth(IEventListener& listener, const std::string& name);
    ~RplethAuth() noexcept = default;

    RplethAuth(const RplethAuth& other) = delete;
    RplethAuth& operator=(const RplethAuth& other) = delete;

public:
    virtual void                notify(const Event& event) override;
    virtual const std::string&  getName() const override;
    virtual ModuleType          getType() const override;
    virtual void                serialize(boost::property_tree::ptree& node) override;
    virtual void                deserialize(boost::property_tree::ptree& node) override;

public:
    void                run();

private:
    void                buildSelectParams();
    void                handleClientMessage(Client& client);
    void                handleCardIdQueue();

private:
    IEventListener&         _listener;
    const std::string       _name;
    std::atomic<bool>       _isRunning;
    std::thread             _networkThread;
    Rezzo::ISocket*         _serverSocket;
    std::queue<CardId>      _cardIdQueue;
    std::mutex              _cardIdQueueMutex;
    Rezzo::ISocket::Port    _port;
    std::list<Client>       _clients;
    fd_set                  _rSet;
    int                     _fdMax;
    long                    _timeout;
    struct timeval          _timeoutStruct;
    Byte                    _buffer[RingBufferSize];
};

#endif // RPLETHAUTH_HPP
