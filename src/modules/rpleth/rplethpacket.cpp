/**
 * \file rplethpacket.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Rpleth packet class
 */

#include "rplethpacket.hpp"

RplethPacket::RplethPacket(Sender packetSender)
:   sender(packetSender)
{}

RplethPacket::RplethPacket(const RplethPacket& other)
:   status(other.status),
    type(other.type),
    command(other.command),
    dataLen(other.dataLen),
    data(other.data),
    sum(other.sum),
    isGood(other.isGood),
    sender(other.sender)
{}

RplethPacket& RplethPacket::operator=(const RplethPacket& other)
{
    status = other.status;
    type = other.type;
    command = other.command;
    dataLen = other.dataLen;
    data = other.data;
    sum = other.sum;
    isGood = other.isGood;
    sender = other.sender;
    return (*this);
}

Byte RplethPacket::checksum() const
{
    Byte s = 0;

    if (sender == Sender::Server)
        s ^= status;
    s ^= type;
    s ^= command;
    s ^= dataLen;
    for (int i = 0; i < dataLen; ++i)
        s ^= data[i];
    return (s);
}
