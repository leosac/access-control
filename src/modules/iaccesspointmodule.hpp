/**
 * \file iaccesspointmodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief interface for AccessPoint classes
 */

#ifndef IACCESSPOINTMODULE_HPP
#define IACCESSPOINTMODULE_HPP

#include "imodule.hpp"

#include <string>

class IAccessPointModule : public IModule
{
public:
    virtual ~IAccessPointModule() {}
    virtual void    notifyAccess(const std::string& request) = 0; // FIXME Remove this call
    virtual void    notifyResponse(bool granted) = 0;
};

#endif // IACCESSPOINTMODULE_HPP
