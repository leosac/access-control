/**
 * \file authcmddenyaccess.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief AuthCmdDenyAccess class implementation
 */

#include "authcmddenyaccess.hpp"
#include "iauthcommandhandler.hpp"

AuthCmdDenyAccess::AuthCmdDenyAccess(IAuthCommandHandler* handlerInstance, AuthRequest::Uid id)
:   AAuthCommand(handlerInstance, id)
{}

void AuthCmdDenyAccess::execute()
{
    _handlerInstance->cmdAuthorize(_id, false);
}
