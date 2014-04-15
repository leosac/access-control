#include "wiegandinterface.hpp"

#include <cstring>
#include <iostream> // FIXME Debug printing
#include <iomanip> // FIXME Debug printing

WiegandInterface::WiegandInterface(IGPIOObservable& gpioProvider)
:   _bitIdx(0)
{
    _hiGpio = 14; // FIXME Debug
    _loGpio = 15; // FIXME Debug
    gpioProvider.registerListener(this, _hiGpio);
    gpioProvider.registerListener(this, _loGpio);
}

WiegandInterface::~WiegandInterface() {}

WiegandInterface::WiegandInterface(const WiegandInterface& /*other*/) {}

WiegandInterface& WiegandInterface::operator=(const WiegandInterface& /*other*/)
{
    return (*this);
}

void WiegandInterface::notify(int gpioNo)
{
    if (_bitIdx == DataBufferLen * 8) // Buffer overflow
        reset();

    if (gpioNo == _hiGpio)
       _buffer[_bitIdx / 8] |= 1 << (8 - _bitIdx % 8);

    ++_bitIdx;
}
 
void WiegandInterface::timeout()
{
    if (_bitIdx)
    {
        // TODO send message
        std::cout << "Read: ";
        for (int i = DataBufferLen - 1; i >= 0; --i)
            std::cout << std::hex << _buffer[i] << ' ';
        std::cout << std::endl;
        reset();
    }
}

void WiegandInterface::reset()
{
    ::memset(_buffer, 0, DataBufferLen);
    _bitIdx = 0;
}
