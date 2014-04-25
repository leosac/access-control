/**
 * \file rplethprotocol.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Rpleth protocol implementation
 */

#include "rplethprotocol.hpp"

#include <iostream> // DEBUG
#include <vector>

RplethProtocol::RplethProtocol() {}

RplethPacket RplethProtocol::decodeCommand(CircularBuffer& buffer)
{
    RplethPacket    packet(RplethPacket::Client);
    std::size_t     toRead = buffer.toRead();

    std::cout << "DEBUG: Packet decode: ToRead size=" << toRead << std::endl;
    packet.status = Success;
    packet.isGood = false;
    if (toRead < PacketMinSize)
        return (packet);

    packet.dataLen = buffer[SizeByteIdx];
    if (toRead < static_cast<std::size_t>(packet.dataLen + 4))
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
    {
        packet.status = BadType;
        std::cout << "DEBUG: Packet type KO" << std::endl;
    }

    else if (packet.sum != packet.checksum())
    {
        packet.status = BadChecksum;
        std::cout << "DEBUG: Checksum KO" << std::endl;
    }
    return (packet);
}

std::size_t RplethProtocol::encodeCommand(const RplethPacket& packet, Byte* buffer, std::size_t /*size*/)
{
    // TODO Compare with input size

    if (packet.sender == RplethPacket::Server)
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

    if (packet.sender == RplethPacket::Server)
        return (packet.dataLen + 4 + 1);
    else
        return (packet.dataLen + 4);
}

RplethPacket RplethProtocol::processClientPacket(const RplethPacket& packet)
{
    RplethPacket response = packet;

    response.sender = RplethPacket::Server;
    if (response.type == Rpleth && response.command == Ping)
        response.status = Success;
    else
        response.status = Success; // NOTE Default response
    return (response);
}
