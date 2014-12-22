#include "IUser.hpp"

using namespace Leosac::Auth;

IUser::IUser(const std::string &user_id) :
        id_(user_id)
{

}

const std::string &IUser::id() const noexcept
{
    return id_;
}

void IUser::id(const std::string &id_new)
{
    id_ = id_new;
}

IAccessProfilePtr IUser::profile() const noexcept
{
    return profile_;
}

void IUser::profile(IAccessProfilePtr user_profile)
{
    profile_ = user_profile;
}
