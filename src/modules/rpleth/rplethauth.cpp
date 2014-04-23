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
    _networkThread = std::thread(&launch, this);
}

RplethAuth::~RplethAuth()
{
    {
        std::lock_guard<std::mutex> lg(_runMutex);
        _isRunning = false;
    }
    _networkThread.join();
}

void RplethAuth::sendEvent(const Event& event)
{
    // TODO
}

IModule::Type RplethAuth::getType() const
{
    return (Authentication);
}

const std::string& RplethAuth::getVersionString() const
{
    return (_version);
}

void RplethAuth::run()
{
    RplethProtocol  protocol;
    Byte            buffer[1024 + 1];
    std::size_t     ret;

    _serverSocket = new Rezzo::UnixSocket(Rezzo::ISocket::TCP);
    _serverSocket->bind(_port);
    _serverSocket->listen();
    _runMutex.lock();
    _isRunning = true;
    while (_isRunning)
    {
        _runMutex.unlock();
        FD_ZERO(&_rSet);
        FD_SET(_serverSocket->getHandle(), &_rSet);
        FD_SET(0, &_rSet);
        _fdMax = _serverSocket->getHandle();
        for (std::list<Rezzo::ISocket*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            _fdMax = std::max(_fdMax, (*it)->getHandle());
            FD_SET((*it)->getHandle(), &_rSet);
        }
        _timeoutStruct.tv_sec = _timeout / 1000;
        _timeoutStruct.tv_usec = (_timeout % 1000) * 1000;
        if ((ret = ::select(_fdMax + 1, &_rSet, nullptr, nullptr, &_timeoutStruct)) == -1)
            throw (ModuleException(UnixSyscall::getErrorString("select", errno)));
        else if (ret > 0)
        {
            for (std::list<Rezzo::ISocket*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
            {
                if (FD_ISSET((*it)->getHandle(), &_rSet))
                {
                    ret = (*it)->recv(buffer, 1024);
                    if (!ret)
                    {
                        delete *it;
                        _clients.erase(it);
                        std::cout << "Client OUT" << std::endl;
                        break;
                    }
                    RplethPacket packet = RplethProtocol::decodeCommand(buffer, ret);
                    RplethPacket response = protocol.processClientPacket(packet);
                    std::size_t size = RplethProtocol::encodeCommand(response, buffer, 1024);
                    (*it)->send(buffer, size);
                }
            }
            if (FD_ISSET(0, &_rSet))
            {
                read(0, buffer, 1024);
                std::cout << "Input Detected" << std::endl;
                Rezzo::ISocket* client = _clients.front();
                RplethPacket csn(RplethPacket::Server);
                csn.type = RplethProtocol::HID;
                csn.command = RplethProtocol::Badge;
                csn.dataLen = 4;
                csn.data = std::vector<Byte>(4, 0xFF);
                csn.sum = csn.checksum();
                std::size_t size = RplethProtocol::encodeCommand(csn, buffer, 1024);
                client->send(buffer, size);
            }
            if (FD_ISSET(_serverSocket->getHandle(), &_rSet))
            {
                _clients.push_back(_serverSocket->accept());
                std::cout << "Client connected" << std::endl;
            }
        }
        _runMutex.lock();
    }
    _runMutex.unlock();
    std::cout << "ENDLOOP" << std::endl;
    for (std::list<Rezzo::ISocket*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        (*it)->close();
        delete *it;
    }
    _serverSocket->close();
    delete _serverSocket;
}
