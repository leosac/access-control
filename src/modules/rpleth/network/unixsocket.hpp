/**
 * \file unixsocket.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Unix socket class
 */

#ifndef UNIXSOCKET_HPP
#define UNIXSOCKET_HPP

#include "isocket.hpp"

namespace Rezzo
{
    class UnixSocket : public ISocket
    {
    public:
        explicit UnixSocket(Protocol protocol);
        ~UnixSocket() = default;

    protected:
        UnixSocket(int handle, Address address);

    public:
        std::size_t recv(Byte* data, std::size_t size, int flags = 0);
        std::size_t send(const Byte* data, std::size_t size, int flags = 0);
        void        connect(const std::string& host, Port port);
        void        bind(Port port);
        void        listen(int backLogSize = 0);
        ISocket*    accept();
        void        close();
        std::string getIp() const;
        Port        getPort() const;
        Address     getAddress() const;
        int         getHandle() const;

    public:
        static ISocket::Ip  resolveHostname(const std::string& host);

    private:
        UnixSocket(const UnixSocket& other);
        UnixSocket& operator=(const UnixSocket& other);

    private:
        int     _handle;
        Address _address;
    };
}

#endif // UNIXSOCKET_HPP
