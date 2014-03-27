/**
 * \file IMutex.h
 * \author Thibault Schueller <thibault.schueller@islog.com>
 * \brief Mutex interface
 */

#ifndef IMUTEX_H
#define IMUTEX_H

class IMutex
{
public:
    virtual ~IMutex() {}
    virtual void  lock() = 0;
    virtual void  unlock() = 0;
    virtual bool  trylock() = 0;
};

#endif // IMUTEX_H
