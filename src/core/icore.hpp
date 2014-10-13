/**
 * \file icore.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief core interface
 */

#ifndef ICORE_HPP
#define ICORE_HPP

#include <string>

#include "hardware/ihwmanager.hpp"
#include "moduleprotocol/imoduleprotocol.hpp"

class ICore
{
public:
    virtual ~ICore() {}
    virtual IHWManager&         getHWManager() = 0;
    virtual IModuleProtocol&    getModuleProtocol() = 0;
    /**
    * Ask the core to reset the application.
    * This will effectively lead to Core object destruction and return from Core::run().
    * The main()'s loop will restart a new core.
    */
    virtual void reset() = 0;
};

#endif // ICORE_HPP
