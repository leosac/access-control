/**
 * \file rplethpacket.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Rpleth packet class
 */

#include "rplethpacket.hpp"

RplethPacket::RplethPacket(Sender packetSender)
:   isGood(false),
    sender(packetSender)
{}

RplethPacket::~RplethPacket() {}

Byte RplethPacket::checksum() const
{
    Byte s = 0;

    if (sender == Server)
        s ^= status;
    s ^= type;
    s ^= command;
    s ^= dataLen;
    for (int i = 0; i < dataLen; ++i)
        s ^= data[i];
    return (s);
}
