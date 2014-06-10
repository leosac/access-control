/**
 * \file iauthprotocol.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief authprotocol interface
 */

#ifndef IAUTHPROTOCOL_HPP
#define IAUTHPROTOCOL_HPP

#include <string>

#include "authrequest.hpp"

class IAuthProtocol
{
public:
    virtual ~IAuthProtocol() {}
    virtual void    logMessage(const std::string& message) = 0;
    virtual void    createAuthRequest(const std::string& request, const std::string& target) = 0;
    virtual void    authorize(AuthRequest::Uid id, bool granted) = 0;
};

#endif // IAUTHPROTOCOL_HPP
