/**
 * \file rplethauth.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief rpleth compatibility module
 */

#include "rplethauth.hpp"

#include <sstream>

#include "network/unixsocket.hpp"
#include "rplethprotocol.hpp"
#include "tools/unixsyscall.hpp"
#include "tools/log.hpp"
#include "tools/unlock_guard.hpp"
#include "exception/moduleexception.hpp"

RplethAuth::RplethAuth(ICore& core, const std::string& name)
:   _core(core),
    _name(name),
    _isRunning(true),
    _serverSocket(nullptr),
    _port(0),
    _fdMax(0),
    _timeout(DefaultTimeoutMs)
{}

const std::string& RplethAuth::getName() const
{
    return (_name);
}

IModule::ModuleType RplethAuth::getType() const
{
    return (ModuleType::Auth);
}

void RplethAuth::serialize(ptree& node)
{
    node.put("port", _port);

    _isRunning = false;
    _networkThread.join();
}

void RplethAuth::deserialize(const ptree& node)
{
    _port = node.get<Rezzo::UnixSocket::Port>("port", Rezzo::ISocket::Port(DefaultPort));
    _networkThread = std::thread([this] () { run(); } );
}

void RplethAuth::authenticate(const AuthRequest& ar)
{
    std::istringstream          iss(ar.getContent());
    CardId                      cid;
    std::uint8_t                byte;
    std::lock_guard<std::mutex> lg(_cardIdQueueMutex);

    while (iss >> byte)
        cid.push_back(static_cast<Byte>(byte));

    _cardIdQueue.push(cid);
    LOG() << "Card id length = " << cid.size();
    LOG() << "Will authorized the cmd with id = " << std::string((char *)&cid[0], cid.size());
    _core.getModuleProtocol().cmdAuthorize(ar.getId(), true);
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
        buildSelectParams();
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
                    try {
                        readRet = it->socket->recv(_buffer, RingBufferSize);
                        it->buffer.write(_buffer, readRet);
                        handleClientMessage(*it);
                    }
                    catch (const ModuleException& e) {
                        it->socket->close();
                        delete it->socket;
                        _clients.erase(it);
                        LOG() << "Client disconnected";
                        break;
                    }
                }
            }
            if (FD_ISSET(_serverSocket->getHandle(), &_rSet))
            {
                _clients.push_back(Client(_serverSocket->accept()));
                LOG() << "Client connected";
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

void RplethAuth::buildSelectParams()
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
}

void RplethAuth::handleClientMessage(RplethAuth::Client& client)
{
    RplethPacket packet(RplethPacket::Sender::Client);

    do
    {
        packet = RplethProtocol::decodeCommand(client.buffer);
        if (!packet.isGood)
            break;
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
        {
            unlock_guard<std::mutex>    ulg(_cardIdQueueMutex);

            packet.dataLen = cid.size();
            packet.data = cid;
            size = RplethProtocol::encodeCommand(packet, _buffer, RingBufferSize);
            for (auto& client : _clients)
                client.socket->send(_buffer, size);
        }
    }
}
