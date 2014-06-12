/**
 * \file authcmddenyaccess.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief AuthCmdDenyAccess class declaration
 */

#ifndef AUTHCMDDENYACCESS_HPP
#define AUTHCMDDENYACCESS_HPP

#include "aauthcommand.hpp"

class AuthCmdDenyAccess : public AAuthCommand
{
public:
    explicit AuthCmdDenyAccess(IAuthCommandHandler* handlerInstance, AuthRequest::Uid id);
    ~AuthCmdDenyAccess() = default;

public:
    virtual void    execute() override;
};

#endif // AUTHCMDDENYACCESS_HPP
