#include "AuthTarget.hpp"

using namespace Leosac::Auth;

const std::string &AuthTarget::name() const
{
    return name_;
}

void AuthTarget::name(std::string const &param)
{
    name_ = param;
}
