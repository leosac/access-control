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
    virtual ~IDevice() {}
    virtual const std::string&  getName() const = 0;
};

#endif // IDEVICE_HPP
