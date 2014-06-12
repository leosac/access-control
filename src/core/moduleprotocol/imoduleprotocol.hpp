/**
 * \file imoduleprotocol.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief moduleprotocol interface
 */

#ifndef IMODULEPROTOCOL_HPP
#define IMODULEPROTOCOL_HPP

#include <string>

#include "authrequest.hpp"
#include "authcommands/aauthcommand.hpp"
#include "authcommands/iauthcommandhandler.hpp"

class IModuleProtocol : public IAuthCommandHandler
{
public:
    virtual ~IModuleProtocol() {}
    virtual void    logMessage(const std::string& message) = 0;
    virtual void    pushAuthCommand(AAuthCommand* command) = 0;
};

#endif // IMODULEPROTOCOL_HPP
