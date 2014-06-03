/**
 * \file wiegandreader.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Wiegand hardware communication layer
 */

#include "wiegandreader.hpp"

#include <cstring>

WiegandReader::WiegandReader(IGPIOObservable& gpioProvider)
:   _gpioProvider(gpioProvider)
{}

void WiegandReader::notify(int gpioNo)
{
    if (_bitIdx >= DataBufferSize * 8) // Buffer overflow
        reset();
    if (gpioNo == _hiGpio)
       _buffer[_bitIdx / 8] |= (1 << (7 - _bitIdx % 8));
    ++_bitIdx;
}

void WiegandReader::timeout()
{
    if (_bitIdx)
    {
        std::size_t                 size = ((_bitIdx - 1) / 8) + 1;
        IWiegandListener::CardId    c(size);

        for (std::size_t i = 0; i < size; ++i)
            c[i] = _buffer[size - i - 1];
        for (auto& listener : _listener)
            listener->notifyCardRead(c);
        reset();
    }
}

IDevice::DeviceType WiegandReader::getType() const
{
    return (IDevice::DeviceType::WiegandReader);
}

void WiegandReader::serialize(boost::property_tree::ptree& node)
{
    node.put<int>("higpio", _hiGpio);
    node.put<int>("logpio", _loGpio);
}

void WiegandReader::deserialize(const boost::property_tree::ptree& node)
{
    _hiGpio = node.get<int>("higpio");
    _loGpio = node.get<int>("logpio");
    _gpioProvider.registerListener(this, _hiGpio, GPIO::Rising);
    _gpioProvider.registerListener(this, _loGpio, GPIO::Rising);
    reset();
}

void WiegandReader::reset()
{
    ::memset(_buffer, 0, DataBufferSize);
    _bitIdx = 0;
}
