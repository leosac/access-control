/**
 * \file imonitormodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief imonitormodule interface
 */

#ifndef IMONITORMODULE_HPP
#define IMONITORMODULE_HPP

#include "imodule.hpp"

class IMonitorModule : public IModule
{
public:
    virtual ~IMonitorModule() {}
};

#endif // IMONITORMODULE_HPP
