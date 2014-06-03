/**
 * \file ihwmanager.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief hardware managing class interface
 */

#ifndef IHWMANAGER_HPP
#define IHWMANAGER_HPP

#include <string>

class IDevice;

class IWiegandListener;
class WiegandInterface;
class GPIO;

class IHWManager
{
public:
    virtual ~IHWManager() {}
    virtual void        start() = 0;
    virtual void        stop() = 0;
    virtual IDevice*    getDevice(const std::string& name) = 0;
};

#endif // IHWMANAGER_HPP
