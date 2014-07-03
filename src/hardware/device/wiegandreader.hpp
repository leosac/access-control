/**
 * \file wiegandreader.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief Wiegand hardware communication layer
 */

#ifndef WIEGANDREADER_HPP
#define WIEGANDREADER_HPP

#include <list>

#include "tools/bufferutils.hpp"
#include "gpio/igpiolistener.hpp"
#include "gpiodevice.hpp"
#include "hardware/iserializabledevice.hpp"

class IWiegandListener;

/* NOTE This class is very likely to be created by the main thread,
 * but controlled by the polling thread
 */

class WiegandReader : public IGPIOListener, public ISerializableDevice
{
    static const int    DataBufferSize = 10;

public:
    explicit WiegandReader(const std::string& name, IGPIOProvider& gpioProvider);
    ~WiegandReader() = default;

    WiegandReader(const WiegandReader& other) = delete;
    WiegandReader& operator=(const WiegandReader& other) = delete;

public:
    virtual void        notify(int gpioNo) override;
    virtual void        timeout() override;

public:
    virtual const std::string&  getName() const override;
    virtual void                serialize(ptree& node) override;
    virtual void                deserialize(const ptree& node) override;

public:
    void    registerListener(IWiegandListener* listener);
    void    unregisterListener(IWiegandListener* listener);

private:
    void    reset();

private:
    const std::string               _name;
    std::list<IWiegandListener*>    _listeners;
    Byte                            _buffer[DataBufferSize];
    std::size_t                     _bitIdx;
    GpioDevice                      _hiGpio;
    GpioDevice                      _loGpio;
};

#endif // WIEGANDREADER_HPP
