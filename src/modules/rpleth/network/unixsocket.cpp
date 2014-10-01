/**
 * \file unixsocket.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Unix socket class
 */

#include "unixsocket.hpp"

extern "C" {
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
}

#include <sstream>
#include <cerrno>
#include <cassert>

#include "tools/unixsyscall.hpp"
#include "exception/moduleexception.hpp"

namespace Rezzo {

UnixSocket::UnixSocket(Protocol protocol)
:   _handle(-1)
{
    struct protoent *proto;
    std::string     protoName;
    int             protoId = 0;
    int             type = 0;
    int             opt = 1;

    if (protocol == ISocket::Protocol::TCP)
    {
        protoName = "TCP";
        type = SOCK_STREAM;
    }
    else if (protocol == ISocket::Protocol::UDP)
    {
        protoName = "UDP";
        type = SOCK_DGRAM;
    }
    assert(protoName.size() == 3);
    if ((proto = getprotobyname(protoName.c_str()))) // fixme: for some reason triggers valgrind invalid read
        protoId = proto->p_proto;
    if ((_handle = ::socket(AF_INET, type, protoId)) == -1)
        throw (ModuleException(UnixSyscall::getErrorString("socket", errno)));
    if (::setsockopt(_handle, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        throw (ModuleException(UnixSyscall::getErrorString("setsockopt", errno)));
}

UnixSocket::UnixSocket(int handle, ISocket::Address address)
:   _handle(handle),
    _address(address)
{}

std::size_t UnixSocket::recv(Byte* data, std::size_t size, int flags)
{
    int ret;

    if ((ret = ::recv(_handle, data, size, flags)) <= 0)
        throw (ModuleException(UnixSyscall::getErrorString("recv", errno)));
    return (ret);
}

std::size_t UnixSocket::send(const Byte* data, std::size_t size, int flags)
{
    int ret;

    if ((ret = ::send(_handle, data, size, flags)) <= 0)
        throw (ModuleException(UnixSyscall::getErrorString("send", errno)));
    return (ret);
}

void UnixSocket::connect(const std::string& host, Port port)
{
    struct sockaddr_in  addr;
    int                 ret;

    _address.ip = resolveHostname(host);
    _address.port = port;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = _address.ip;
    if ((ret = ::connect(_handle, reinterpret_cast<const struct sockaddr*>(&addr), sizeof(addr)) == -1))
        throw (ModuleException(UnixSyscall::getErrorString("connect", errno)));
}

void UnixSocket::bind(Port port)
{
    struct sockaddr_in  addr;

    _address.ip = resolveHostname("0.0.0.0");
    _address.port = port;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = _address.ip;
    addr.sin_port = htons(_address.port);
    if (::bind(_handle, reinterpret_cast<const struct sockaddr*>(&addr), sizeof(addr)) == -1)
        throw (ModuleException(UnixSyscall::getErrorString("bind", errno)));
}

void UnixSocket::listen(int backLogSize)
{
    int ret;

    if (!backLogSize)
        backLogSize = SOMAXCONN;
    if ((ret = ::listen(_handle, backLogSize)) == -1)
        throw (ModuleException(UnixSyscall::getErrorString("listen", errno)));
}

ISocket* UnixSocket::accept()
{
    struct sockaddr_in  addr;
    Address             clientAddr;
    socklen_t           size;
    int                 handle;

    size = sizeof(addr);
    if ((handle = ::accept(_handle, reinterpret_cast<struct sockaddr*>(&addr), &size)) == -1)
        throw (ModuleException(UnixSyscall::getErrorString("accept", errno)));
    clientAddr.port = htons(addr.sin_port);
    clientAddr.ip = addr.sin_addr.s_addr;
    return (new UnixSocket(handle, clientAddr));
}

void UnixSocket::close()
{
    int ret;

    if ((ret = ::close(_handle)) == -1)
        throw (ModuleException(UnixSyscall::getErrorString("close", errno)));
    _handle = -1;
}

std::string UnixSocket::getIp() const
{
    std::ostringstream  oss;
    Ip                  ip = _address.ip;

    for (int i = 0; i < 4; ++i)
    {
        oss << (ip & 0xff);
        ip >>= 8;
        if (i < 3)
            oss << '.';
    }
    return (oss.str());
}

ISocket::Port UnixSocket::getPort() const
{
    return (_address.port);
}

ISocket::Address UnixSocket::getAddress() const
{
    return (_address);
}

int UnixSocket::getHandle() const
{
    return (_handle);
}

ISocket::Ip UnixSocket::resolveHostname(const std::string& host)
{
    struct hostent*       he;
    struct in_addr**      addr_list;

    if (!(he = ::gethostbyname(host.c_str())))
        return (0);
    addr_list = reinterpret_cast<struct in_addr**>(he->h_addr_list);
    if (!addr_list[0])
        return (0);
    return (addr_list[0]->s_addr);
}

    UnixSocket::~UnixSocket()
    {
        if (_handle != -1)
        {
            try
            {
                close();
            }
            catch (ModuleException &e)
            {
                
            }
        }
    }
}
