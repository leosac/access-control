/**
 * \file wiegandinterface.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Wiegand hardware communication layer
 */

#ifndef WIEGANDINTERFACE_HPP
#define WIEGANDINTERFACE_HPP

#include <map>

#include "gpio/igpiolistener.hpp"
#include "gpio/igpioobservable.hpp"
#include "tools/bufferutils.hpp"

/* NOTE This class is very likely to be created by the main thread,
 * but controlled by the polling thread
 */

class WiegandInterface : public IGPIOListener
{
    static const int    DataBufferSize = 10;
public:
    WiegandInterface(IGPIOObservable& gpioProvider);
    ~WiegandInterface();

private:
    WiegandInterface(const WiegandInterface& other);
    WiegandInterface& operator=(const WiegandInterface& other);

public:
    void    notify(int gpioNo);
    void    timeout();

private:
    void    reset();
    void    debugPrint();

private:
    Byte        _buffer[DataBufferSize];
    std::size_t _bitIdx;
    int         _hiGpio;
    int         _loGpio;
};

#endif // WIEGANDINTERFACE_HPP
