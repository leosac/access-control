/**
 * \file icore.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief core interface
 */

#ifndef ICORE_HPP
#define ICORE_HPP

#include "modules/ieventlistener.hpp"
#include "hardware/ihwmanager.hpp"

class ICore : public IEventListener
{
public:
    virtual ~ICore() {}
    virtual IHWManager& getHWManager() = 0;
};

#endif // ICORE_HPP
