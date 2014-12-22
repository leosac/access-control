#include <tools/log.hpp>
#include "AuthTarget.hpp"

using namespace Leosac::Auth;

const std::string &AuthTarget::name() const
{
    return name_;
}

void AuthTarget::name(std::string const &param)
{
    NOTICE("Changing target name (from " << name_ << " to " << param);
    name_ = param;
}

AuthTarget::AuthTarget(const std::string target_name) :
        name_(target_name)
{

}
