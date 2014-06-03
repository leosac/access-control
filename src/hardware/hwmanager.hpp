/**
 * \file hwmanager.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief hardware managing class
 */

#ifndef HWMANAGER_HPP
#define HWMANAGER_HPP

#include "ihwmanager.hpp"
#include "config/xmlconfig.hpp"
#include "config/ixmlserializable.hpp"

#include "iserializabledevice.hpp"
#include "device/gpio/gpiomanager.hpp"
#include "device/iwiegandlistener.hpp"

class HWManager : public IHWManager, public IXmlSerializable
{
    typedef struct {
        std::string             type;
        ISerializableDevice*    instance;
    } Device;

public:
    explicit HWManager() = default;
    ~HWManager() = default;

    HWManager(const HWManager& other) = delete;
    HWManager& operator=(const HWManager& other) = delete;

public:
    virtual void serialize(boost::property_tree::ptree& node) override;
    virtual void deserialize(const boost::property_tree::ptree& node) override;

public:
    virtual void        start() override;
    virtual void        stop() override;
    virtual IDevice*    getDevice(const std::string& name) override;

private:
    ISerializableDevice*    buildDevice(const std::string& type);

private:
    GPIOManager                     _gpioManager;
    std::map<std::string, Device>   _devices;
};

#endif // HWMANAGER_HPP
