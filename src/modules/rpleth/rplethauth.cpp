/**
 * \file rplethauth.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief rpleth compatibility module
 */

#include "rplethauth.hpp"

#include <iostream>
#include <unistd.h> // FIXME rm this

#include "network/unixsocket.hpp"
#include "rplethprotocol.hpp"
#include "tools/version.hpp"
#include "tools/unixsyscall.hpp"
#include "exception/moduleexception.hpp"

static void launch(RplethAuth* instance)
{
    instance->run();
}

RplethAuth::RplethAuth(Rezzo::ISocket::Port port, long timeoutMs)
:   _version(Version::buildVersionString(0, 1, 0)),
    _isRunning(false),
    _serverSocket(nullptr),
    _port(port),
    _fdMax(0),
    _timeout(timeoutMs)
{
    _isRunning = true;
    _networkThread = std::thread(&launch, this);
}

RplethAuth::~RplethAuth()
{
    _isRunning = false;
    _networkThread.join();
}

void RplethAuth::sendAuthRequest(const IAuthModule::AuthRequest& /*request*/)
{
    // TODO
}

IModule::Type RplethAuth::getType() const
{
    return (Auth);
}

const std::string& RplethAuth::getVersionString() const
{
    return (_version);
}

void RplethAuth::run()
{
    std::size_t     readRet;
    int             selectRet;

    _serverSocket = new Rezzo::UnixSocket(Rezzo::ISocket::TCP);
    _serverSocket->bind(_port);
    _serverSocket->listen();
    while (_isRunning)
    {
        FD_ZERO(&_rSet);
        FD_SET(_serverSocket->getHandle(), &_rSet);
        _fdMax = _serverSocket->getHandle();
        for (std::list<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            _fdMax = std::max(_fdMax, it->socket->getHandle());
            FD_SET(it->socket->getHandle(), &_rSet);
        }
        _timeoutStruct.tv_sec = _timeout / 1000;
        _timeoutStruct.tv_usec = (_timeout % 1000) * 1000;
        if ((selectRet = ::select(_fdMax + 1, &_rSet, nullptr, nullptr, &_timeoutStruct)) == -1)
            throw (ModuleException(UnixSyscall::getErrorString("select", errno)));
        else if (selectRet > 0)
        {
            for (std::list<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
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
                        std::cout << "Client disconnected" << std::endl;
                        break;
                    }
                }
            }
            if (FD_ISSET(_serverSocket->getHandle(), &_rSet))
            {
                _clients.push_back(Client(_serverSocket->accept()));
                std::cout << "Client connected" << std::endl;
            }
        }
    }
    for (std::list<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        it->socket->close();
        delete it->socket;
    }
    _serverSocket->close();
    delete _serverSocket;
}

void RplethAuth::handleClientMessage(RplethAuth::Client& client)
{
    RplethPacket packet(RplethPacket::Client);

    do
    {
        packet = RplethProtocol::decodeCommand(client.buffer);
        if (!packet.isGood)
            break;
        std::cout << "Packet received (s=" << packet.dataLen + 4 << ')' << std::endl;
        RplethPacket response = RplethProtocol::processClientPacket(packet);
        std::size_t size = RplethProtocol::encodeCommand(response, _buffer, RingBufferSize);
        client.socket->send(_buffer, size);
    }
    while (packet.isGood && client.buffer.toRead());
}
