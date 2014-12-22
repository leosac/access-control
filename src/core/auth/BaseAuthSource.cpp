#include <tools/IVisitor.hpp>
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

IAccessProfilePtr BaseAuthSource::profile() const
{
    if (owner_)
        return owner_->profile();
    return nullptr;
}

void BaseAuthSource::accept(Leosac::Tools::IVisitor *visitor)
{
    visitor->visit(this);
}

const std::string &BaseAuthSource::name() const
{
    return source_name_;
}

void BaseAuthSource::name(std::string const &n)
{
    source_name_ = n;
}
