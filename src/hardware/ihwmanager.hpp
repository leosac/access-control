/**
 * \file ihwmanager.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief hardware managing class interface
 */

#ifndef IHWMANAGER_HPP
#define IHWMANAGER_HPP

class IWiegandListener;
class WiegandInterface;
class GPIO;

class IHWManager
{
public:
    virtual ~IHWManager() noexcept = default;
    virtual void    start() = 0;
    virtual void    stop() = 0;
    virtual WiegandInterface*   buildWiegandInterface(IWiegandListener* listener, unsigned int hiGpioIdx, unsigned int loGpioIdx) = 0;
    virtual GPIO*               buildGPIO(int idx) = 0;
};

#endif // IHWMANAGER_HPP
