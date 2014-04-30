/**
 * \file wiegandinterface.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Wiegand hardware communication layer
 */

#include "wiegandinterface.hpp"

#include <cstring>
#include <iostream> // FIXME Debug printing
#include <iomanip> // FIXME Debug printing

WiegandInterface::WiegandInterface(IGPIOObservable& gpioProvider, IWiegandListener* listener)
:   _listener(listener)
{
    _hiGpio = 15; // FIXME Debug
    _loGpio = 14; // FIXME Debug
    gpioProvider.registerListener(this, _hiGpio, GPIO::Rising);
    gpioProvider.registerListener(this, _loGpio, GPIO::Rising);
    reset();
}

WiegandInterface::~WiegandInterface() {}

void WiegandInterface::notify(int gpioNo)
{
    if (_bitIdx >= DataBufferSize * 8) // Buffer overflow
        reset();

    if (gpioNo == _hiGpio)
       _buffer[_bitIdx / 8] |= (1 << (7 - _bitIdx % 8));

    ++_bitIdx;
}

void WiegandInterface::timeout()
{
    if (_bitIdx)
    {
        std::size_t                 size = (_bitIdx - 1 / 8) + 1;
        IWiegandListener::CardId    c(size);

        for (std::size_t i = 0; i < size; ++i)
            c[i] = _buffer[i];

        _listener->notifyCardRead(c);
        debugPrint();
        reset();
    }
}

void WiegandInterface::reset()
{
    ::memset(_buffer, 0, DataBufferSize);
    _bitIdx = 0;
}

void WiegandInterface::debugPrint()
{
    std::cout << "Read: ";
    for (int i = (_bitIdx - 1) / 8; i >= 0; --i)
    {
        for (int j = 7; j >= 0; --j)
            std::cout << ((_buffer[i] & (1 << j)) > 0);
        std::cout << ' ';
    }
    std::cout << "(bits = " << _bitIdx << ")" << std::endl;
}
