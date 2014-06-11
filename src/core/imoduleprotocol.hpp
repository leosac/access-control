/**
 * \file imoduleprotocol.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief moduleprotocol interface
 */

#ifndef IMODULEPROTOCOL_HPP
#define IMODULEPROTOCOL_HPP

#include <string>

#include "authrequest.hpp"

class IModuleProtocol
{
public:
    virtual ~IModuleProtocol() {}
    virtual void    logMessage(const std::string& message) = 0;
    virtual void    createAuthRequest(const std::string& request, const std::string& target) = 0;
    virtual void    authorize(AuthRequest::Uid id, bool granted) = 0;
};

#endif // IMODULEPROTOCOL_HPP
