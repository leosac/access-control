/**
 * \file icore.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief core interface
 */

#ifndef ICORE_HPP
#define ICORE_HPP

#include <string>

#include "hardware/ihwmanager.hpp"
#include "authrequest.hpp"

class ICore
{
public:
    virtual ~ICore() {}
    virtual IHWManager& getHWManager() = 0;
    virtual void        sendAuthRequest(const std::string& request) = 0;
    virtual void        authorize(AuthRequest::Uid id, bool granted) = 0;
};

#endif // ICORE_HPP
