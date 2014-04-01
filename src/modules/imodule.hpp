/**
 * \file imodule.hpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief module class interface
 */

#ifndef IMODULE_HPP
#define IMODULE_HPP

class IModule
{
public:
    virtual ~IModule() {}
    virtual void    sayHello() const = 0;
};

#endif // IMODULE_HPP
