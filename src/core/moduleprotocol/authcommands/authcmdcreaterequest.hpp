/**
 * \file authcmdcreaterequest.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief AuthCmdCreateRequest class declaration
 */

#ifndef AUTHCMDCREATEREQUEST_HPP
#define AUTHCMDCREATEREQUEST_HPP

#include "aauthcommand.hpp"

class AuthCmdCreateRequest : public AAuthCommand
{
public:
    explicit AuthCmdCreateRequest(IAuthCommandHandler* handlerInstance, const std::string& source, const std::string& target, const std::string& content);
    ~AuthCmdCreateRequest() = default;

public:
    virtual void    execute() override;

private:
    std::string _source;
    std::string _target;
    std::string _content;
};

#endif // AUTHCMDCREATEREQUEST_HPP
