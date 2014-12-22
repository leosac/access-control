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
