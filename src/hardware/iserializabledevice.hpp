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
    virtual const std::string&  getName() const override = 0;
    virtual void                serialize(ptree& node) override = 0;
    virtual void                deserialize(const ptree& node) override = 0;
};

#endif // ISERIALIZABLEDEVICE_HPP
