/**
 * \file rplethpacket.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Rpleth packet class
 */

#ifndef RPLETHPACKET_HPP
#define RPLETHPACKET_HPP

#include "tools/bufferutils.hpp"

#include <vector>

class RplethPacket
{
public:
    enum Sender {
        Client = 0,
        Server
    };

public:
    RplethPacket(Sender packetSender);
    ~RplethPacket();

public:
    Byte    checksum() const;

public:
    Byte                status;
    Byte                type;
    Byte                command;
    Byte                dataLen;
    std::vector<Byte>   data;
    Byte                sum;
    bool                isGood;
    Sender              sender;
};

#endif // RPLETHPACKET_HPP
