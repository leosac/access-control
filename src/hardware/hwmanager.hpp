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
    explicit HWManager() = default;
    ~HWManager() = default;

    HWManager(const HWManager& other) = delete;
    HWManager& operator=(const HWManager& other) = delete;

public:
    virtual void                start() override;
    virtual void                stop() override;
    virtual WiegandInterface*   buildWiegandInterface(IWiegandListener* listener, unsigned int hiGpioIdx, unsigned int loGpioIdx) override;
    virtual GPIO*               buildGPIO(int idx) override;

private:
#ifndef NO_HW
    GPIOManager         _gpioManager; // FIXME Prefer iface
#endif
};

#endif // HWMANAGER_HPP
