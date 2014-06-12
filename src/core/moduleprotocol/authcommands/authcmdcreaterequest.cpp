/**
 * \file authcmdcreaterequest.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief AuthCmdCreateRequest class implementation
 */

#include "authcmdcreaterequest.hpp"
#include "iauthcommandhandler.hpp"

AuthCmdCreateRequest::AuthCmdCreateRequest(IAuthCommandHandler* handlerInstance, const std::string& source, const std::string& target, const std::string& content)
:   AAuthCommand(handlerInstance, 0),
    _source(source),
    _target(target),
    _content(content)
{}

void AuthCmdCreateRequest::execute()
{
    _handlerInstance->cmdCreateAuthRequest(_source, _target, _content);
}
