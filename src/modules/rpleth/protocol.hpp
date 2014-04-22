/**
 * \file protocol.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Rpleth protocol implementation
 */

#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include "tools/bufferutils.hpp"
#include "rplethpacket.hpp"

class Protocol
{
    Protocol();
public:
    // Type codes
    static const Byte RPLETH            = 0x00;
    static const Byte HID               = 0x01;
    static const Byte LCD               = 0x02;

    // Cmd codes
    static const Byte STATEDHCP         = 0x01;
    static const Byte DHCP              = 0x02;
    static const Byte MAC               = 0x03;
    static const Byte IP                = 0x04;
    static const Byte SUBNET            = 0x05;
    static const Byte GATEWAY           = 0x06;
    static const Byte PORT              = 0x07;
    static const Byte MESSAGE           = 0x08;
    static const Byte RESET             = 0x09;
    static const Byte PING              = 0x0a;

    static const Byte BEEP              = 0x00;
    static const Byte GREENLED          = 0x01;
    static const Byte REDLED            = 0x02;
    static const Byte NOP               = 0x03;
    static const Byte BADGE             = 0x04;
    static const Byte COM               = 0x05;
    static const Byte WAIT_INSERTION    = 0x06;
    static const Byte WAIT_REMOVAL      = 0x07;
    static const Byte CONNECT           = 0x08;
    static const Byte DISCONNECT        = 0x09;
    static const Byte GET_READERTYPE    = 0x0a;
    static const Byte GET_CSN           = 0x0b;
    static const Byte SET_CARDTYPE      = 0x0c;

    static const Byte DISPLAY           = 0x00;
    static const Byte DISPLAYT          = 0x01;
    static const Byte BLINK             = 0x02;
    static const Byte SCROLL            = 0x03;
    static const Byte DISPLAYTIME       = 0x04;

public:
    static RplethPacket    decode(const Byte* buffer, std::size_t size);
};

#endif // PROTOCOL_HPP
