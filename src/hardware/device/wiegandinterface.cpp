#include "wiegandinterface.hpp"

#include <cstring>
#include <iostream> // FIXME Debug printing
#include <iomanip> // FIXME Debug printing

WiegandInterface::WiegandInterface(IGPIOObservable& gpioProvider)
:   _bitIdx(0)
{
    _hiGpio = 14; // FIXME Debug
    _loGpio = 15; // FIXME Debug
    gpioProvider.registerListener(this, _hiGpio, GPIO::Rising);
    gpioProvider.registerListener(this, _loGpio, GPIO::Rising);
}

WiegandInterface::~WiegandInterface() {}

WiegandInterface::WiegandInterface(const WiegandInterface& /*other*/) {}

WiegandInterface& WiegandInterface::operator=(const WiegandInterface& /*other*/)
{
    return (*this);
}

void WiegandInterface::notify(int gpioNo)
{
    if (_bitIdx >= DataBufferLen * 8) // Buffer overflow
        reset();

    if (gpioNo == _hiGpio)
       _buffer[_bitIdx / 8] |= (1 << (_bitIdx % 8));

    ++_bitIdx;
}

void WiegandInterface::timeout()
{
    if (_bitIdx)
    {
        // TODO send message
        debugPrint();
        reset();
    }
}

void WiegandInterface::reset()
{
    ::memset(_buffer, 0, DataBufferLen);
    _bitIdx = 0;
}

void WiegandInterface::debugPrint()
{
    std::cout << "Read: ";
    for (int i = _bitIdx / 8; i >= 0; --i)
    {
        for (int j = 7; j >= 0; --j)
            std::cout << ((_buffer[i] & (1 << j)) > 0);
        std::cout << ' ';
    }
    std::cout << "(bits = " << _bitIdx << ")" << std::endl;
}
