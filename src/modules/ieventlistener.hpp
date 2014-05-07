/**
 * \file ieventlistener.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief
 */

#ifndef IEVENTLISTENER_HPP
#define IEVENTLISTENER_HPP

#include "core/event.hpp"

class IEventListener
{
public:
    virtual ~IEventListener() = default;
    virtual void    notify(const Event& event) = 0;
};

#endif // IEVENTLISTENER_HPP
