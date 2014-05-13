/**
 * \file authrequest.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief AuthRequest class implementation
 */

#include "authrequest.hpp"

AuthRequest::Uid AuthRequest::UidCounter = 0;

AuthRequest::AuthRequest()
:   _uid(UidCounter)
{
    ++UidCounter;
}

AuthRequest::Uid AuthRequest::getUid() const
{
    return (_uid);
}
