/**
 * \file iloggermodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief interface for the logger module
 */

#ifndef ILOGGERMODULE_HPP
#define ILOGGERMODULE_HPP

#include "imodule.hpp"

class ILoggerModule : public IModule
{
public:
    virtual ~ILoggerModule() {}
};

#endif // ILOGGERMODULE_HPP
