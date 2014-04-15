#ifndef WIEGANDINTERFACE_HPP
#define WIEGANDINTERFACE_HPP

#include <map>

#include "gpio/igpiolistener.hpp"
#include "tools/bufferutils.hpp"

class WiegandInterface : public IGPIOListener
{
    static const int    DataBufferLen = 64;
public:
    WiegandInterface();
    ~WiegandInterface();

private:
    WiegandInterface(const WiegandInterface& other);
    WiegandInterface& operator=(const WiegandInterface& other);

public:
    void    notify(int gpioNo);

private:
    void    reset();

public:
    Byte        _buffer[DataBufferLen];
    std::size_t _bitIdx;
    int         _hiGpio;
    int         _loGpio;
};

#endif // WIEGANDINTERFACE_HPP
