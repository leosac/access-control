/**
 * \file idevice.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief hardware device interface
 */

#ifndef IDEVICE_HPP
#define IDEVICE_HPP

#include <string>

class IDevice
{
public:
    enum class DeviceType {
        Led,
        Button,
        Wiegand
    };

public:
    virtual ~IDevice() {}
    virtual DeviceType  getType() const = 0;
};

#endif // IDEVICE_HPP
