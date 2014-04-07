/**
 * \file iactivitymonitormodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief interface for the activity monitor module
 */

#ifndef IACTIVITYMONITORMODULE_HPP
#define IACTIVITYMONITORMODULE_HPP

class IActivityMonitor
{
public:
    virtual ~IActivityMonitor() {}
    virtual void    lol() = 0;
};

#endif // IACTIVITYMONITORMODULE_HPP
