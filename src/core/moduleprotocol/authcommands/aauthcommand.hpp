/**
 * \file aauthcommand.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief AAuthCommand abstract class declaration
 */

#ifndef AAUTHCOMMAND_HPP
#define AAUTHCOMMAND_HPP

#include "core/icommand.hpp"
#include "core/moduleprotocol/authrequest.hpp"

class IAuthCommandHandler;

class AAuthCommand : public ICommand
{
public:
    explicit AAuthCommand(IAuthCommandHandler* handlerInstance, AuthRequest::Uid id);
    virtual ~AAuthCommand() = default;

protected:
    IAuthCommandHandler*    _handlerInstance;
    AuthRequest::Uid        _id;
};

#endif // AAUTHCOMMAND_HPP
