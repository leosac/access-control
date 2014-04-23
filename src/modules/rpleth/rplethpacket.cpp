/**
 * \file rplethpacket.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Rpleth packet class
 */

#include "rplethpacket.hpp"

RplethPacket::RplethPacket(Sender sender)
:   isGood(false),
    sender(sender)
{}

RplethPacket::~RplethPacket() {}

Byte RplethPacket::checksum() const
{
    Byte sum = 0;

    if (sender == Server)
        sum ^= status;
    sum ^= type;
    sum ^= command;
    sum ^= dataLen;
    for (int i = 0; i < dataLen; ++i)
        sum ^= data[i];
    return (sum);
}
