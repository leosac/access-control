/**
 * \file rplethprotocol.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Rpleth protocol implementation
 */

#include "rplethprotocol.hpp"

#include <iostream> // DEBUG
#include <vector>

RplethProtocol::RplethProtocol() {}

RplethProtocol::~RplethProtocol() {}

RplethPacket RplethProtocol::decodeCommand(const Byte* buffer, std::size_t size)
{
    RplethPacket    packet(RplethPacket::Client);

    std::cout << "DEBUG: Packet received size=" << size << std::endl;

    packet.status = Success;
    if (size < PacketMinSize)
    {
        packet.status = BadSize;
        std::cout << "DEBUG: Packet min size KO" << std::endl;
        return (packet);
    }
    packet.isGood = true; // The packet has enough information to be interpreted by the protocol

    packet.type = buffer[TypeByteIdx];
    if (packet.type >= MaxType)
    {
        packet.status = BadType;
        std::cout << "DEBUG: Packet type KO" << std::endl;
        return (packet);
    }

    packet.command = buffer[CommandByteIdx];
    packet.dataLen = buffer[SizeByteIdx];

    if (size != (packet.dataLen + 4))
    {
        packet.status = BadSize;
        std::cout << "DEBUG: Packet size KO" << std::endl;
        return (packet);
    }

    if (packet.dataLen)
    {
        packet.data = std::vector<Byte>(packet.dataLen);
        for (unsigned int i = 0; i < packet.dataLen; ++i)
            packet.data[i] = buffer[SizeByteIdx + 1 + i];
    }

    packet.sum = buffer[SizeByteIdx + packet.dataLen + 1];
    if (packet.sum != packet.checksum())
    {
        packet.status = BadChecksum;
        std::cout << "DEBUG: Checksum KO" << std::endl;
        return (packet);
    }

    return (packet);
}

std::size_t RplethProtocol::encodeCommand(const RplethPacket& packet, Byte* buffer, std::size_t size)
{
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

//     if (!packet.isGood || packet.sender != RplethPacket::Client)

    response.sender = RplethPacket::Server;
    if (response.type == Rpleth && response.command == Ping)
    {
        response.status = Success;
    }
    else
    {
        response.status = Success; // NOTE Default response
    }
    return (response);
}
