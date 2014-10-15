/**
 * \file circularbuffer.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief simple circular buffer class
 */

#ifndef CIRCULARBUFFER_HPP
#define CIRCULARBUFFER_HPP

#include <vector>
#include <string>

#include "tools/bufferutils.hpp"

class CircularBuffer
{
    static const std::size_t    DefaultSize = 8192;

public:
    CircularBuffer(std::size_t size = DefaultSize);
    ~CircularBuffer() = default;

public:
    std::size_t read(Byte* data, std::size_t size);
    std::size_t write(const Byte* data, std::size_t size);
    Byte        operator[](int idx) const;
    void        fastForward(std::size_t offset);
    void        reset();
    std::size_t getSize() const;
    std::size_t toRead() const;
    bool        isEmpty() const;

private:
    std::vector<Byte>   _buffer;
    std::size_t         _size;
    std::size_t         _rIdx;
    std::size_t         _wIdx;
    std::size_t         _toRead;
};

#endif // CIRCULARBUFFER_HPP
