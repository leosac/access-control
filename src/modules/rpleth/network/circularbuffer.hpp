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
    static const std::size_t    DefaultSize = 100;

public:
    CircularBuffer(std::size_t size = DefaultSize);
    ~CircularBuffer() = default;

public:
    std::size_t read(Byte* data, std::size_t size) noexcept;
    std::size_t write(const Byte* data, std::size_t size) noexcept;
    Byte        operator[](int idx) const noexcept;
    void        fastForward(std::size_t offset) noexcept;
    void        reset() noexcept;
    std::size_t getSize() const noexcept;
    std::size_t toRead() const noexcept;
    bool        isEmpty() const noexcept;

private:
    std::vector<Byte>   _buffer;
    std::size_t         _size;
    std::size_t         _rIdx;
    std::size_t         _wIdx;
    std::size_t         _toRead;
};

#endif // CIRCULARBUFFER_HPP
