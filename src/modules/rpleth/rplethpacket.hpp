/*
    Copyright (C) 2014-2016 Islog

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
* \file rplethpacket.hpp
* \author Thibault Schueller <ryp.sqrt@gmail.com>
* \brief Rpleth packet class
*/

#ifndef RPLETHPACKET_HPP
#define RPLETHPACKET_HPP

#include "tools/bufferutils.hpp"

#include <vector>

namespace Leosac
{
namespace Module
{
namespace Rpleth
{
class RplethPacket
{
  public:
    enum class Sender
    {
        Client = 0,
        Server
    };

  public:
    RplethPacket(Sender packetSender);

    RplethPacket(const RplethPacket &other);

    ~RplethPacket() = default;

    RplethPacket &operator=(const RplethPacket &other);

  public:
    Byte checksum() const;

  public:
    Byte status;
    Byte type;
    Byte command;
    Byte dataLen;
    std::vector<Byte> data;
    Byte sum;
    bool isGood;
    Sender sender;
};
}
}
}

#endif // RPLETHPACKET_HPP
