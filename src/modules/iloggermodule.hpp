/**
 * \file iloggermodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief iloggermodule interface
 */

#ifndef ILOGGERMODULE_HPP
#define ILOGGERMODULE_HPP

#include "imodule.hpp"

class ILoggerModule : public IModule
{
public:
    virtual ~ILoggerModule() {}
    virtual void    log(const std::string& message) = 0;
};

#endif // ILOGGERMODULE_HPP
