/**
 * \file wiegandreader.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Wiegand hardware communication layer
 */

#include "wiegandreader.hpp"

#include <cstring>
#include <bitset>

#include "iwiegandlistener.hpp"
#include "tools/log.hpp"

WiegandReader::WiegandReader(const std::string& name, IGPIOObservable& gpioObservable)
:   _name(name),
    _gpioObservable(gpioObservable)
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
        {
            c[i] = _buffer[size - i - 1];
            std::bitset<8> a(c[i]);
            LOG() << '[' << i << "] "<< a;
        }
        for (auto& listener : _listeners)
            listener->notifyCardRead(c);
        reset();
    }
}

const std::string& WiegandReader::getName() const
{
    return (_name);
}

void WiegandReader::serialize(ptree& node)
{
    node.put<int>("higpio", _hiGpio);
    node.put<int>("logpio", _loGpio);
}

void WiegandReader::deserialize(const ptree& node)
{
    _hiGpio = node.get<int>("higpio");
    _loGpio = node.get<int>("logpio");
    _gpioObservable.registerListener(this, _hiGpio, GPIO::EdgeMode::Rising);
    _gpioObservable.registerListener(this, _loGpio, GPIO::EdgeMode::Rising);
    reset();
}

void WiegandReader::registerListener(IWiegandListener* listener)
{
    _listeners.push_back(listener);
}

void WiegandReader::unregisterListener(IWiegandListener* listener)
{
    _listeners.remove(listener);
}

void WiegandReader::reset()
{
    ::memset(_buffer, 0, DataBufferSize);
    _bitIdx = 0;
}
