/**
 * \file hwmanager.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief hardware managing class
 */

#ifndef HWMANAGER_HPP
#define HWMANAGER_HPP

#include "ihwmanager.hpp"
#include "device/gpio/gpiomanager.hpp"
#include "device/wiegandinterface.hpp"

class HWManager : public IHWManager
{
public:
    HWManager();
    ~HWManager();

public:
    void    start();
    void    stop();

private:
    GPIOManager         _gpioManager;
    WiegandInterface    _wiegand;
};

#endif // HWMANAGER_HPP
