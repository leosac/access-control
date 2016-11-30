/*
    Copyright (C) 2014-2016 Leosac

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
* \file circularbuffer.hpp
* \author Thibault Schueller <ryp.sqrt@gmail.com>
* \brief simple circular buffer class
*/

#ifndef CIRCULARBUFFER_HPP
#define CIRCULARBUFFER_HPP

#include <string>
#include <vector>

#include "tools/bufferutils.hpp"

namespace Leosac
{
namespace Module
{
namespace Rpleth
{
/**
* Implementation of a ring buffer.
*/
class CircularBuffer
{
    static const std::size_t DefaultSize = 8192;

  public:
    CircularBuffer(std::size_t size = DefaultSize);

    ~CircularBuffer() = default;

  public:
    std::size_t read(Byte *data, std::size_t size);

    std::size_t write(const Byte *data, std::size_t size);

    Byte operator[](int idx) const;

    void fastForward(std::size_t offset);

    void reset();

    std::size_t getSize() const;

    std::size_t toRead() const;

    bool isEmpty() const;

  private:
    std::vector<Byte> _buffer;
    std::size_t _size;
    std::size_t _rIdx;
    std::size_t _wIdx;
    std::size_t _toRead;
};
}
}
}
#endif // CIRCULARBUFFER_HPP
