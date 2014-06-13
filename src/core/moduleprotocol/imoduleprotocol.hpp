/**
 * \file imoduleprotocol.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief moduleprotocol interface
 */

#ifndef IMODULEPROTOCOL_HPP
#define IMODULEPROTOCOL_HPP

#include <string>

#include "authrequest.hpp"
#include "authcommands/iauthcommandhandler.hpp"

class AAuthCommand;

class IModuleProtocol : public IAuthCommandHandler
{
public:
    enum class ActivityType {
        System,
        Auth
    };

public:
    virtual ~IModuleProtocol() {}
    virtual void    logMessage(const std::string& message) = 0;
    virtual void    notifyMonitor(ActivityType type) = 0;
    virtual void    pushAuthCommand(AAuthCommand* command) = 0;
};

#endif // IMODULEPROTOCOL_HPP
