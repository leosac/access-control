/**
 * \file rplethauth.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief rpleth compatibility module
 */

#include "rplethauth.hpp"

#include <sstream>

#include "network/unixsocket.hpp"
#include "rplethprotocol.hpp"
#include "tools/version.hpp"
#include "tools/unixsyscall.hpp"
#include "exception/moduleexception.hpp"

static void launch(RplethAuth* instance)
{
    instance->run();
}

RplethAuth::RplethAuth(IEventListener& listener, Rezzo::ISocket::Port port, long timeoutMs)
:   _listener(listener),
    _version(Version::buildVersionString(0, 1, 0)),
    _isRunning(true),
    _serverSocket(nullptr),
    _port(port),
    _fdMax(0),
    _timeout(timeoutMs)
{
    _networkThread = std::thread(&launch, this);
}

RplethAuth::~RplethAuth()
{
    _isRunning = false;
    _networkThread.join();
}

void RplethAuth::notify(const Event& event)
{
    std::stringstream   ss(event.message);
    CardId              cid;
    unsigned int        val;

    while (ss >> val)
        cid.push_back(static_cast<Byte>(val));

    std::lock_guard<std::mutex> lg(_cardIdQueueMutex);
    _cardIdQueue.push(cid);
}

IModule::ModuleType RplethAuth::getType() const
{
    return (ModuleType::Auth);
}

const std::string& RplethAuth::getVersionString() const
{
    return (_version);
}

void RplethAuth::run()
{
    std::size_t     readRet;
    int             selectRet;

    _serverSocket = new Rezzo::UnixSocket(Rezzo::ISocket::Protocol::TCP);
    _serverSocket->bind(_port);
    _serverSocket->listen();
    while (_isRunning)
    {
        FD_ZERO(&_rSet);
        FD_SET(_serverSocket->getHandle(), &_rSet);
        _fdMax = _serverSocket->getHandle();
        for (auto& client : _clients)
        {
            _fdMax = std::max(_fdMax, client.socket->getHandle());
            FD_SET(client.socket->getHandle(), &_rSet);
        }
        _timeoutStruct.tv_sec = _timeout / 1000;
        _timeoutStruct.tv_usec = (_timeout % 1000) * 1000;
        if ((selectRet = ::select(_fdMax + 1, &_rSet, nullptr, nullptr, &_timeoutStruct)) == -1)
            throw (ModuleException(UnixSyscall::getErrorString("select", errno)));
        else if (!selectRet)
            handleCardIdQueue();
        else
        {
            for (auto it = _clients.begin(); it != _clients.end(); ++it)
            {
                if (FD_ISSET(it->socket->getHandle(), &_rSet))
                {
                    try
                    {
                        readRet = it->socket->recv(_buffer, RingBufferSize);
                        it->buffer.write(_buffer, readRet);
                        handleClientMessage(*it);
                    }
                    catch (const ModuleException& e)
                    {
                        it->socket->close();
                        delete it->socket;
                        _clients.erase(it);
                        _listener.notify(Event("Client disconnected", "Auth")); // FIXME Debug
                        break;
                    }
                }
            }
            if (FD_ISSET(_serverSocket->getHandle(), &_rSet))
            {
                _clients.push_back(Client(_serverSocket->accept()));
                _listener.notify(Event("Client connected", "Auth")); // FIXME Debug
            }
        }
    }
    for (auto& client : _clients)
    {
        client.socket->close();
        delete client.socket;
    }
    _serverSocket->close();
    delete _serverSocket;
}

void RplethAuth::handleClientMessage(RplethAuth::Client& client)
{
    RplethPacket packet(RplethPacket::Sender::Client);

    do
    {
        packet = RplethProtocol::decodeCommand(client.buffer);
        if (!packet.isGood)
            break;
//         std::cout << "Packet received (s=" << packet.dataLen + 4 << ')' << std::endl; // FIXME Debug
        RplethPacket response = RplethProtocol::processClientPacket(packet);
        std::size_t size = RplethProtocol::encodeCommand(response, _buffer, RingBufferSize);
        client.socket->send(_buffer, size);
    }
    while (packet.isGood && client.buffer.toRead());
}

void RplethAuth::handleCardIdQueue()
{
    CardId                      cid;
    RplethPacket                packet(RplethPacket::Sender::Server);
    std::lock_guard<std::mutex> lg(_cardIdQueueMutex);
    std::size_t                 size;

    packet.status = RplethProtocol::Success;
    packet.type = RplethProtocol::HID;
    packet.command = RplethProtocol::Badge;
    while (!_cardIdQueue.empty())
    {
        cid = _cardIdQueue.front();
        _cardIdQueue.pop();
        _cardIdQueueMutex.unlock();
        packet.dataLen = cid.size();
        packet.data = cid;
        size = RplethProtocol::encodeCommand(packet, _buffer, RingBufferSize);
        for (auto& client : _clients)
            client.socket->send(_buffer, size);
        _listener.notify(Event("Open the door !", "RplethAuth", "Door"));
        _cardIdQueueMutex.lock();
    }
}
