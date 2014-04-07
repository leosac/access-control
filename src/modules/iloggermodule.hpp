/**
 * \file iloggermodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief interface for the logger module
 */

#ifndef ILOGGERMODULE_HPP
#define ILOGGERMODULE_HPP

#include "event.hpp"

class ILoggerModule
{
public:
    virtual ~ILoggerModule() {}
    virtual void    sendEvent(const Event& event) = 0;
};

#endif // ILOGGERMODULE_HPP
