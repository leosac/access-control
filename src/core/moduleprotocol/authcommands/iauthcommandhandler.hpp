/**
 * \file iauthcommandhandler.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief IAuthCommandHandler class declaration
 */

#ifndef IAUTHCOMMANDHANDLER_HPP
#define IAUTHCOMMANDHANDLER_HPP

#include <string>

#include "core/moduleprotocol/authrequest.hpp"

class IAuthCommandHandler
{
public:
    virtual ~IAuthCommandHandler() {}
    virtual void    cmdCreateAuthRequest(const std::string& source, const std::string& target, const std::string& content) = 0;
    virtual void    cmdAuthorize(AuthRequest::Uid id, bool granted) = 0;
};

#endif // IAUTHCOMMANDHANDLER_HPP
