/**
 * \file iwiegandlistener.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief
 */

#ifndef IWIEGANDLISTENER_HPP
#define IWIEGANDLISTENER_HPP

#include <vector>

#include "tools/bufferutils.hpp"

class IWiegandListener
{
public:
    typedef std::vector<Byte> CardId;

public:
    virtual ~IWiegandListener() {}
    virtual void    notifyCardRead(const CardId& cardId) = 0;
};

#endif // IWIEGANDLISTENER_HPP
