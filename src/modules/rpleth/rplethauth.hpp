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
#include <hardware/idevice.hpp>

#include "modules/iauthmodule.hpp"
#include "network/isocket.hpp"
#include "network/circularbuffer.hpp"

class RplethAuth : public IAuthModule
{
    static const Rezzo::ISocket::Port   DefaultPort = 9559;
    static const long                   DefaultTimeoutMs = 500;
    static const std::size_t            RingBufferSize = 512;

    struct Client {
        Rezzo::ISocket* socket;
        CircularBuffer  buffer;
        explicit Client(Rezzo::ISocket* sock) : socket(sock), buffer(RingBufferSize) {}
    };

public:
    using CardId = std::vector<Byte>;

public:
    explicit RplethAuth(ICore& core, const std::string& name);
    ~RplethAuth() = default;

    RplethAuth(const RplethAuth& other) = delete;
    RplethAuth& operator=(const RplethAuth& other) = delete;

public:
    virtual const std::string&  getName() const override;
    virtual ModuleType          getType() const override;
    virtual void                serialize(ptree& node) override;
    virtual void                deserialize(const ptree& node) override;
    virtual void                authenticate(const AuthRequest& ar) override;

private:
    void    run();
    void    buildSelectParams();
    void    handleClientMessage(Client& client);

    /**
    * This method will flush the cardIdQueue to the connected clients, effectively notifying them of
    * activity by sending the card id that was read.
    */
    void    handleCardIdQueue();

private:
    ICore&                  _core;
    const std::string       _name;
    std::atomic<bool>       _isRunning;
    std::thread             _networkThread;
    Rezzo::ISocket*         _serverSocket;

    /**
    * This a queue of all card ID yet to be send over the network.
    */
    std::queue<CardId>      _cardIdQueue;
    std::mutex              _cardIdQueueMutex;

    /**
    * Network port we bind to, and listen to client.
    */
    Rezzo::ISocket::Port    _port;
    IDevice                 *greenLed_;
    IDevice                 *buzzer_;

public:
    /**
    * Retrieve the buzzer. May be null.
    */
    IDevice *getBuzzer() const;

    /**
    * Retrieve a pointer to the device handling the Green Led device.
    * This pointer MAY be null.
    */
    IDevice * getGreenLed() const;

private:
    std::list<Client>       _clients;
    fd_set                  _rSet;
    int                     _fdMax;
    long                    _timeout;
    struct timeval          _timeoutStruct;
    Byte                    _buffer[RingBufferSize];
};

#endif // RPLETHAUTH_HPP
