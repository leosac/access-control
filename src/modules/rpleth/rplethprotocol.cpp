/**
 * \file rplethprotocol.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Rpleth protocol implementation
 */

#include "rplethprotocol.hpp"

#include <vector>

RplethPacket RplethProtocol::decodeCommand(CircularBuffer& buffer)
{
    RplethPacket    packet(RplethPacket::Sender::Client);
    std::size_t     toRead = buffer.toRead();

    packet.status = Success;
    packet.isGood = false;
    if (toRead < PacketMinSize)
        return (packet);

    packet.dataLen = buffer[SizeByteIdx];
    if (toRead < packet.dataLen + 4U)
        return (packet);
    if (packet.dataLen)
    {
        packet.data = std::vector<Byte>(packet.dataLen);
        for (unsigned int i = 0; i < packet.dataLen; ++i)
            packet.data[i] = buffer[SizeByteIdx + 1 + i];
    }
    packet.type = buffer[TypeByteIdx];
    packet.command = buffer[CommandByteIdx];
    packet.sum = buffer[SizeByteIdx + packet.dataLen + 1];
    packet.isGood = true; // The packet has enough information to be interpreted by the protocol
    buffer.fastForward(4 + packet.dataLen); // Circular buffer was actually read but indexes were not updated

    if (packet.type >= MaxType)
        packet.status = BadType;
    else if (packet.sum != packet.checksum())
        packet.status = BadChecksum;
    return (packet);
}

std::size_t RplethProtocol::encodeCommand(const RplethPacket& packet, Byte* buffer, std::size_t size)
{
    if (size < packet.dataLen + 5U) // Buffer is too small
        return (0);
    if (packet.sender == RplethPacket::Sender::Server)
    {
        buffer[0] = packet.status;
        ++buffer;
    }
    buffer[TypeByteIdx] = packet.type;
    buffer[CommandByteIdx] = packet.command;
    buffer[SizeByteIdx] = packet.dataLen;
    for (int i = 0; i < packet.dataLen; ++i)
        buffer[SizeByteIdx + i + 1] = packet.data[i];
    buffer[SizeByteIdx + packet.dataLen + 1] = packet.checksum();

    if (packet.sender == RplethPacket::Sender::Server)
        return (packet.dataLen + 4 + 1);
    else
        return (packet.dataLen + 4);
}

RplethPacket RplethProtocol::processClientPacket(const RplethPacket& packet)
{
    RplethPacket response = packet;

    response.sender = RplethPacket::Sender::Server;
    if (response.type == Rpleth && response.command == Ping)
        response.status = Success;
    else
        response.status = Success; // Default response
    return (response);
}
