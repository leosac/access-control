/*
    Copyright (C) 2014-2022 Leosac

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
* \file rplethprotocol.hpp
* \brief Rpleth protocol implementation
*/

#ifndef RPLETHPROTOCOL_HPP
#define RPLETHPROTOCOL_HPP

#include "network/circularbuffer.hpp"
#include "rplethpacket.hpp"

#include <queue>

namespace Leosac
{
namespace Module
{
namespace Rpleth
{

class RplethProtocol
{
    static const std::size_t TypeByteIdx    = 0;
    static const std::size_t CommandByteIdx = 1;
    static const std::size_t SizeByteIdx    = 2;
    static const std::size_t PacketMinSize  = 4;

    RplethProtocol() = delete;

  public:
    enum TypeCode
    {
        Rpleth = 0x00,
        HID    = 0x01,
        LCD    = 0x02,
        MaxType
    };
    enum RplethCommands
    {
        DHCPState  = 0x01,
        SetDHCP    = 0x02,
        SetMAC     = 0x03,
        SetIP      = 0x04,
        SetSubnet  = 0x05,
        SetGateway = 0x06,
        SetPort    = 0x07,
        Message    = 0x08, // NOTE not documented
        Reset      = 0x09, // This will actually trigger a restart, not a reset.
        Ping       = 0x0a
    };

    enum HIDCommands
    {
        Beep               = 0x00,
        Greenled           = 0x01,
        Redled             = 0x02,
        Nop                = 0x03,
        Badge              = 0x04,
        Com                = 0x05,
        Wait_insertion     = 0x06,
        Wait_removal       = 0x07,
        Connect            = 0x08,
        Disconnect         = 0x09,
        GetReaderType      = 0x0a,
        GetCSN             = 0x0b,
        SetCardType        = 0x0c,
        SendCards          = 0x0d, // text, separated by a pipe
        ReceiveCardsWaited = 0x0e
    };

    enum StatusCode
    {
        Success     = 0x00,
        Failed      = 0x01,
        BadChecksum = 0x02,
        Timeout     = 0x03,
        BadSize     = 0x04,
        BadType     = 0x05,
        MaxStatus
    };

  public:
    /**
    * Decode a packet from a circular buffer object.
    * If `from_server` is true that means the packet comes from a Rpleth server: this
    * is used by unit testing code.
    */
    static RplethPacket decodeCommand(CircularBuffer &buffer,
                                      bool from_server = false);

    static std::size_t encodeCommand(const RplethPacket &packet, Byte *buffer,
                                     std::size_t size);
};
}
}
}
#endif // RPLETHPROTOCOL_HPP
