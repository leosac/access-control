/**
 * \file idoormodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief interface for Door classes
 */

#ifndef IDOORMODULE_HPP
#define IDOORMODULE_HPP

#include "imodule.hpp"

#include <string>
#include <vector>

class IDoorModule : public IModule
{
public:
    virtual ~IDoorModule() {}
    virtual void    open() = 0;
    virtual bool    isAuthRequired() const = 0;
    virtual bool    isOpen() const = 0;
    virtual void    alarm() = 0;
};

#endif // IDOORMODULE_HPP
