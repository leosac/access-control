/**
* \file circularbuffer.cpp
* \author Thibault Schueller <ryp.sqrt@gmail.com>
* \brief simple circular buffer class
*/

#include "circularbuffer.hpp"

#include <algorithm>

using namespace Leosac::Module::Rpleth;

CircularBuffer::CircularBuffer(std::size_t size)
        : _buffer(size),
          _size(size),
          _rIdx(0),
          _wIdx(0),
          _toRead(0)
{
}

std::size_t CircularBuffer::read(Byte *data, std::size_t size)
{
    std::size_t readIdx;

    if (!size || size > _size)
        return (0);
    if (!_toRead)
        return (0);
    size = std::min(size, _toRead);
    for (readIdx = 0; readIdx < size; ++readIdx)
        data[readIdx] = _buffer[(_rIdx + readIdx) % _size];
    if (readIdx == size)
        return (0);
    ++readIdx;
    _rIdx += readIdx;
    _rIdx %= _size;
    _toRead -= readIdx;
    return (readIdx);
}

std::size_t CircularBuffer::write(const Byte *data, std::size_t size)
{
    if (!size || size > _size)
        return (0);
    for (std::size_t i = 0; i < size; ++i)
        _buffer[((_wIdx + i) % _size)] = data[i];
    _wIdx += size;
    _wIdx %= _size;
    if (_rIdx == _wIdx)
        _toRead = _size;
    else
        _toRead = ((_wIdx - _rIdx) + _size) % _size;
    return (size);
}

Byte CircularBuffer::operator[](int idx) const
{
    return (_buffer[(_rIdx + idx) % _size]);
}

void CircularBuffer::fastForward(std::size_t offset)
{
    if (offset > _toRead)
        offset = _toRead;
    _rIdx = (_rIdx + offset) % _size;
    _toRead -= offset;
}

void CircularBuffer::reset()
{
    _rIdx = 0;
    _wIdx = 0;
    _toRead = 0;
}

std::size_t CircularBuffer::getSize() const
{
    return (_size);
}

std::size_t CircularBuffer::toRead() const
{
    return (_toRead);
}

bool CircularBuffer::isEmpty() const
{
    return (_toRead > 0);
}
