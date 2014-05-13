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
#include "iwiegandlistener.hpp"
#include "tools/bufferutils.hpp"

/* NOTE This class is very likely to be created by the main thread,
 * but controlled by the polling thread
 */

class WiegandInterface : public IGPIOListener
{
    static const int    DataBufferSize = 10;

public:
    explicit WiegandInterface(IGPIOObservable& gpioProvider, IWiegandListener* listener, unsigned int hiGpioIdx, unsigned int loGpioIdx);
    ~WiegandInterface() = default;

    WiegandInterface(const WiegandInterface& other) = delete;
    WiegandInterface& operator=(const WiegandInterface& other) = delete;

public:
    virtual void    notify(int gpioNo) override;
    virtual void    timeout() override;

private:
    void    reset();
    void    debugPrint();

private:
    IWiegandListener*   _listener;
    Byte                _buffer[DataBufferSize];
    std::size_t         _bitIdx;
    int                 _hiGpio;
    int                 _loGpio;
};

#endif // WIEGANDINTERFACE_HPP
