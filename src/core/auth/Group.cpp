#include "Group.hpp"

Leosac::Auth::Group::Group(const std::string &group_name) :
        name_(group_name)
{

}

const std::string &Leosac::Auth::Group::name() const
{
    return name_;
}

std::vector<Leosac::Auth::IUserPtr> const &Leosac::Auth::Group::members() const
{
    return members_;
}

void Leosac::Auth::Group::member_add(Leosac::Auth::IUserPtr m)
{
    members_.push_back(m);
}

Leosac::Auth::IAccessProfilePtr Leosac::Auth::Group::profile()
{
    return profile_;
}

void Leosac::Auth::Group::profile(Leosac::Auth::IAccessProfilePtr p)
{
    profile_ = p;
}
