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
