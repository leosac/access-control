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
* \file rplethpacket.cpp
* \brief Rpleth packet class
*/

#include "rplethpacket.hpp"

using namespace Leosac::Module::Rpleth;

RplethPacket::RplethPacket(Sender packetSender)
    : status(0)
    , type(0)
    , command(0)
    , dataLen(0)
    , sum(0)
    , isGood(false)
    , sender(packetSender)
{
}

RplethPacket::RplethPacket(const RplethPacket &other)
    : status(other.status)
    , type(other.type)
    , command(other.command)
    , dataLen(other.dataLen)
    , data(other.data)
    , sum(other.sum)
    , isGood(other.isGood)
    , sender(other.sender)
{
}

RplethPacket &RplethPacket::operator=(const RplethPacket &other)
{
    status  = other.status;
    type    = other.type;
    command = other.command;
    dataLen = other.dataLen;
    data    = other.data;
    sum     = other.sum;
    isGood  = other.isGood;
    sender  = other.sender;
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
