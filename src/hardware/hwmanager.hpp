/**
 * \file hwmanager.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief hardware managing class
 */

#ifndef HWMANAGER_HPP
#define HWMANAGER_HPP

#include "ihwmanager.hpp"
#include "device/gpiomanager.hpp"

class HWManager : public IHWManager
{
public:
    HWManager();
    ~HWManager();

public:
    GPIO*   reserveGPIO(int id);

private:
    GPIOManager _gpioManager;
};

#endif // HWMANAGER_HPP
