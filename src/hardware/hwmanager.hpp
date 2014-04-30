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
#include "device/iwiegandlistener.hpp"

class HWManager : public IHWManager
{
public:
    explicit HWManager();
    ~HWManager();

    HWManager(const HWManager& other) = delete;
    HWManager& operator=(const HWManager& other) = delete;

public:
    void    start();
    void    stop();

public:
    WiegandInterface*   buildWiegandInterface(IWiegandListener* listener);

private:
    GPIOManager         _gpioManager;
};

#endif // HWMANAGER_HPP
