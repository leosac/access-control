/**
 * \file isignalcallback.hpp
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief signal callback interface to use with signalhandler
 */

#ifndef ISIGNALCALLBACK_HPP
#define ISIGNALCALLBACK_HPP

class ISignalCallback
{
public:
    virtual ~ISignalCallback() {}
    virtual void    handleSignal(int signal) = 0;
};

#endif // ISIGNALCALLBACK_HPP
