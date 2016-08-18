/*
    Copyright (C) 2014-2015 Islog

    This file is part of Leosac.

    Leosac is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

/**
* \file rplethprotocol.cpp
* \author Thibault Schueller <ryp.sqrt@gmail.com>
* \brief Rpleth protocol implementation
*/

#include "rplethprotocol.hpp"
#include <vector>
#include <tools/log.hpp>

using namespace Leosac::Module::Rpleth;

RplethPacket RplethProtocol::decodeCommand(CircularBuffer &buffer,
                                           bool from_server /* = false */)
{
    RplethPacket packet(RplethPacket::Sender::Client);
    if (from_server)
    {
        // this is used for unit testing
        buffer.fastForward(1);
        packet.sender = RplethPacket::Sender::Server;
    }
    std::size_t toRead = buffer.toRead();

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
            packet.data[i]  = buffer[SizeByteIdx + 1 + i];
    }
    packet.type    = buffer[TypeByteIdx];
    packet.command = buffer[CommandByteIdx];
    packet.sum     = buffer[SizeByteIdx + packet.dataLen + 1];
    packet.isGood =
        true; // The packet has enough information to be interpreted by the protocol
    buffer.fastForward(4 + packet.dataLen); // Circular buffer was actually read but
                                            // indexes were not updated
    if (packet.type >= MaxType)
        packet.status = BadType;
    else if (packet.sum != packet.checksum())
        packet.status = BadChecksum;
    return (packet);
}

std::size_t RplethProtocol::encodeCommand(const RplethPacket &packet, Byte *buffer,
                                          std::size_t size)
{
    if (size < packet.dataLen + 5U) // Buffer is too small
        return (0);
    if (packet.sender == RplethPacket::Sender::Server)
    {
        buffer[0] = packet.status;
        ++buffer;
    }
    buffer[TypeByteIdx]    = packet.type;
    buffer[CommandByteIdx] = packet.command;
    buffer[SizeByteIdx]    = packet.dataLen;
    for (int i                               = 0; i < packet.dataLen; ++i)
        buffer[SizeByteIdx + i + 1]          = packet.data[i];
    buffer[SizeByteIdx + packet.dataLen + 1] = packet.checksum();

    if (packet.sender == RplethPacket::Sender::Server)
        return (packet.dataLen + 4 + 1);
    else
        return (packet.dataLen + 4);
}
