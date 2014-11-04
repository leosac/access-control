#include "BaseAuthSource.hpp"

using namespace Leosac::Auth;

void BaseAuthSource::addAuthSource(IAuthenticationSourcePtr source)
{
    subsources_.push_back(source);
}

void BaseAuthSource::owner(IUserPtr user)
{
    owner_ = user;
}

IUserPtr BaseAuthSource::owner() const
{
    return owner_;
}
