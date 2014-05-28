/**
 * \file iserializabledevice.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief iserializabledevice interface
 */

#ifndef ISERIALIZABLEDEVICE_HPP
#define ISERIALIZABLEDEVICE_HPP

#include "idevice.hpp"
#include "config/ixmlserializable.hpp"

class ISerializableDevice : public IDevice, public IXmlSerializable
{
public:
    virtual ~ISerializableDevice() {}
    virtual DeviceType  getType() const override = 0;
    virtual void        serialize(boost::property_tree::ptree& node) override = 0;
    virtual void        deserialize(const boost::property_tree::ptree& node) override = 0;
};

#endif // ISERIALIZABLEDEVICE_HPP
