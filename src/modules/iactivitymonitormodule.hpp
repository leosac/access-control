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
    virtual ~IActivityMonitor() = default;
    virtual void    lol() = 0; // FIXME Debug
};

#endif // IACTIVITYMONITORMODULE_HPP
