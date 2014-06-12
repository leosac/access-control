/**
 * \file aauthcommand.cpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief AAuthCommand abstract class implementation
 */

#include "aauthcommand.hpp"

#include "iauthcommandhandler.hpp"

AAuthCommand::AAuthCommand(IAuthCommandHandler* handlerInstance, AuthRequest::Uid id)
:   _handlerInstance(handlerInstance),
    _id(id)
{}
