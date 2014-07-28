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
    using CardId = std::vector<Byte>;

public:
    virtual ~IWiegandListener() {}
    virtual void    notifyCardRead(const CardId& cardId) = 0;
};

#endif // IWIEGANDLISTENER_HPP
