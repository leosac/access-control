/**
 * \file protocol.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Rpleth protocol implementation
 */

#include "protocol.hpp"

Protocol::Protocol() {}

RplethPacket Protocol::decode(const Byte* buffer, std::size_t size)
{
    RplethPacket    packet;

    packet.isGood = false;

    // TODO

    return (packet);
}
