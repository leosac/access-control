/**
 * \file dipswitch.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief DIPSwitch class declaration
 */

#ifndef DIPSWITCH_HPP
#define DIPSWITCH_HPP

#include <functional>

#include "hardware/iserializabledevice.hpp"
#include "gpio/igpioprovider.hpp"
#include "gpiodevice.hpp"

class DIPSwitch : public ISerializableDevice
{
public:
    using SwitchCallback = std::function<void(bool)>;

public:
    explicit DIPSwitch(const std::string& name, IGPIOProvider& gpioProvider);
    ~DIPSwitch() = default;

    DIPSwitch(const DIPSwitch& other) = delete;
    DIPSwitch& operator=(const DIPSwitch& other) = delete;

public:
    virtual const std::string&  getName() const override;
    virtual void                serialize(ptree& node) override;
    virtual void                deserialize(const ptree& node) override;

public:
    std::size_t getSize() const;
    void        setCallback(std::size_t idx, SwitchCallback callback);
    void        readSwitches();

private:
    const std::string           _name;
    IGPIOProvider&              _gpioProvider;
    std::vector<GpioDevice>     _gpioSwitches;
    std::vector<SwitchCallback> _callbacks;
};

#endif // DIPSWITCH_HPP
