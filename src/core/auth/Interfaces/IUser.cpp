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

IUser::IUser(const std::string &username)
    : username_(username)
{
}

const std::string &IUser::username() const noexcept
{
    return username_;
}

void IUser::username(const std::string &username)
{
    username_ = username;
}

IAccessProfilePtr IUser::profile() const noexcept
{
    return profile_;
}

void IUser::profile(IAccessProfilePtr user_profile)
{
    profile_ = user_profile;
}

const std::string &IUser::firstname() const
{
    return firstname_;
}

const std::string &IUser::lastname() const
{
    return lastname_;
}

const std::string &IUser::email() const
{
    return email_;
}

void IUser::firstname(std::string const &f)
{
    firstname_ = f;
}

void IUser::lastname(std::string const &l)
{
    lastname_ = l;
}

void IUser::email(std::string const &e)
{
    email_ = e;
}

const CredentialValidity &IUser::validity() const
{
    return validity_;
}

void IUser::validity(const CredentialValidity &c)
{
    validity_ = c;
}

bool IUser::is_valid() const
{
    return validity_.is_valid();
}

const std::string &IUser::password() const noexcept
{
    return password_;
}

unsigned long IUser::id() const noexcept
{
    return id_;
}
