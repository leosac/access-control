/**
 * \file IMutex.h
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief Mutex interface
 */

#ifndef IMUTEX_HPP
#define IMUTEX_HPP

class IMutex
{
public:
    virtual ~IMutex() {}
    virtual void    lock() = 0;
    virtual void    unlock() = 0;
    virtual bool    trylock() = 0;
};

#endif // IMUTEX_HPP
