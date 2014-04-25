/**
 * \file iauthmodule.hpp
 * \author Thibault Schueller <ryp.sqrt@gmail.com>
 * \brief interface for Auth classes
 */

#ifndef IAUTHMODULE_HPP
#define IAUTHMODULE_HPP

#include "imodule.hpp"

#include <string>
#include <vector>

class IAuthModule : public IModule
{
public:
    typedef struct {
        int id;
    } AuthRequest;

public:
    virtual ~IAuthModule() {}
    virtual void    sendAuthRequest(const AuthRequest& request) = 0;
};

#endif // IAUTHMODULE_HPP
