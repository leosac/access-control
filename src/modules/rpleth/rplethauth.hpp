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
#include <hardware/device/led.hpp>

#include "modules/iauthmodule.hpp"
#include "network/isocket.hpp"
#include "network/circularbuffer.hpp"

/**
* This is a AuthModule that (partially) implements Rpleth.
*
* @note
* All access request are ACCEPTED. The card ID is extracted from
* the AuthRequest, and send to connected clients.

* @note
* This module supports ONE wiegand reader. It can control the green led
* and the buzzer of the device (by handling request from remote client).
*
* Configuration example:
*
* in `hardware.xml`:
* @verbatim
<device type="led" name="wiegand_green_led">
    <gpio pin="3" activelow="1"/>
</device>
<device type="led" name="wiegand_buzzer">
  <!-- This a the buzzer (BEEP) for the wiegand read. Led is the
  device that make most sense -->
  <gpio pin="25" activelow="1" />
</device>
@endverbatim
*
* in `core.xml`:
* @verbatim
 <module file="librpleth.so">
     <alias>rpleth_frontend</alias>
     <properties>
        <port>4242</port>
        <greenLed>wiegand_green_led</greenLed>
        <buzzer>wiegand_buzzer</buzzer>
      </properties>
 </module>
 @endverbatim
*/
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

    /**
    * Controls the reader's buzzer and led and play some melody, letting user know
    * the software is running.
    */
    void    play_test_card_melody();

    /**
    * Called when detecting the reset card (hardcoded card number).
    */
    void    reset_application();

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
    Led                 *greenLed_;
    Led                 *buzzer_;

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
