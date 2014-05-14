/**
 * \file wiegandinterface.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Wiegand hardware communication layer
 */

#include "wiegandinterface.hpp"

#include <cstring>

WiegandInterface::WiegandInterface(IGPIOObservable& gpioProvider, IWiegandListener* listener, unsigned int hiGpioIdx, unsigned int loGpioIdx)
:   _listener(listener),
    _hiGpio(hiGpioIdx),
    _loGpio(loGpioIdx)
{
    gpioProvider.registerListener(this, _hiGpio, GPIO::Rising);
    gpioProvider.registerListener(this, _loGpio, GPIO::Rising);
    reset();
}

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
        std::size_t                 size = ((_bitIdx - 1) / 8) + 1;
        IWiegandListener::CardId    c(size);

        for (std::size_t i = 0; i < size; ++i)
            c[i] = _buffer[size - i - 1];

        _listener->notifyCardRead(c);
        reset();
    }
}

void WiegandInterface::reset()
{
    ::memset(_buffer, 0, DataBufferSize);
    _bitIdx = 0;
}
