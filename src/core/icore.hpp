/**
 * \file icore.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief core interface
 */

#ifndef ICORE_HPP
#define ICORE_HPP

#include "hardware/ihwmanager.hpp"

class ICore
{
public:
    virtual ~ICore() {}
    virtual IHWManager& getHWManager() = 0;
//     virtual void    sendAuthRequest(AuthRequest& request);
};

#endif // ICORE_HPP
