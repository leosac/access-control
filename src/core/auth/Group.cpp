/*
    Copyright (C) 2014-2015 Islog

    This file is part of Leosac.

    Leosac is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leosac is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Group.hpp"

Leosac::Auth::Group::Group(const std::string &group_name)
    : name_(group_name)
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
