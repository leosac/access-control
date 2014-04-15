#include "wiegandinterface.hpp"

#include <cstring>

WiegandInterface::WiegandInterface()
:   _bitIdx(0)
{
    _hiGpio = 15; // FIXME DEBUG test
    _loGpio = 18; // FIXME DEBUG test
}

WiegandInterface::~WiegandInterface() {}

WiegandInterface::WiegandInterface(const WiegandInterface& /*other*/) {}

WiegandInterface& WiegandInterface::operator=(const WiegandInterface& /*other*/)
{
    return (*this);
}

void WiegandInterface::notify(int gpioNo)
{
    if (gpioNo == _hiGpio)
        _buffer[_bitIdx / 8] |= 1 << (_bitIdx % 8);
    ++_bitIdx;
}

void WiegandInterface::reset()
{
    ::memset(_buffer, 0, DataBufferLen);
    _bitIdx = 0;
}
