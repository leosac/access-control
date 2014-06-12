/**
 * \file authcmdgrantaccess.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief AuthCmdGrantAccess class implementation
 */

#include "authcmdgrantaccess.hpp"
#include "iauthcommandhandler.hpp"

AuthCmdGrantAccess::AuthCmdGrantAccess(IAuthCommandHandler* handlerInstance, AuthRequest::Uid id)
:   AAuthCommand(handlerInstance, id)
{}

void AuthCmdGrantAccess::execute()
{
    _handlerInstance->cmdAuthorize(_id, true);
}
