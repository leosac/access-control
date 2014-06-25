/**
 * \file agpiodevice.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief AGpioDevice class declaration
 */

#ifndef AGPIODEVICE_HPP
#define AGPIODEVICE_HPP

#include "hardware/iserializabledevice.hpp"
#include "gpio/igpioprovider.hpp"

class GPIO;

class AGpioDevice : public ISerializableDevice
{
public:
    explicit AGpioDevice(const std::string& name, IGPIOProvider& gpioProvider);
    ~AGpioDevice() = default;

    AGpioDevice(const AGpioDevice& other) = delete;
    AGpioDevice& operator=(const AGpioDevice& other) = delete;

public:
    virtual const std::string&  getName() const override;
    virtual void                serialize(ptree& node) override;
    virtual void                deserialize(const ptree& node) override;

protected:
    GPIO*               _gpio;

private:
    const std::string   _name;
    IGPIOProvider&      _gpioProvider;
    int                 _gpioNo;
};

#endif // AGPIODEVICE_HPP
