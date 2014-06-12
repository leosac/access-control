/**
 * \file authcmdgrantaccess.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief AuthCmdGrantAccess class declaration
 */

#ifndef AUTHCMDGRANTACCESS_HPP
#define AUTHCMDGRANTACCESS_HPP

#include "aauthcommand.hpp"

class AuthCmdGrantAccess : public AAuthCommand
{
public:
    explicit AuthCmdGrantAccess(IAuthCommandHandler* handlerInstance, AuthRequest::Uid id);
    ~AuthCmdGrantAccess() = default;

public:
    virtual void    execute() override;
};

#endif // AUTHCMDGRANTACCESS_HPP
