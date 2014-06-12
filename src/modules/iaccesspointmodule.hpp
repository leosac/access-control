/**
 * \file iaccesspointmodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief interface for AccessPoint classes
 */

#ifndef IACCESSPOINTMODULE_HPP
#define IACCESSPOINTMODULE_HPP

#include "imodule.hpp"

#include <string>
#include <vector>

class IAccessPointModule : public IModule
{
public:
    virtual ~IAccessPointModule() {}
};

#endif // IACCESSPOINTMODULE_HPP
