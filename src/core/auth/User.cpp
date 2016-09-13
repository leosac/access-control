/*
    Copyright (C) 2014-2016 Islog

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

#include "core/auth/User.hpp"
#include "Group_odb.h"
#include "User_odb.h"
#include "core/auth/Group.hpp"
#include "tools/log.hpp"

using namespace Leosac::Auth;

User::User()
    : version_(0)
{
}

User::User(const std::string &username)
    : username_(username)
    , version_(0)
{
}

const std::string &User::username() const noexcept
{
    return username_;
}

void User::username(const std::string &username)
{
    username_ = username;
}

IAccessProfilePtr User::profile() const noexcept
{
    return profile_;
}

void User::profile(IAccessProfilePtr user_profile)
{
    profile_ = user_profile;
}

const std::string &User::firstname() const
{
    return firstname_;
}

const std::string &User::lastname() const
{
    return lastname_;
}

const std::string &User::email() const
{
    return email_;
}

void User::firstname(std::string const &f)
{
    firstname_ = f;
}

void User::lastname(std::string const &l)
{
    lastname_ = l;
}

void User::email(std::string const &e)
{
    email_ = e;
}

const CredentialValidity &User::validity() const
{
    return validity_;
}

void User::validity(const CredentialValidity &c)
{
    validity_ = c;
}

bool User::is_valid() const
{
    return validity_.is_valid();
}

unsigned long User::id() const noexcept
{
    return id_;
}

void User::password(const std::string &pw)
{
    std::vector<uint8_t> vec(pw.begin(), pw.end());
    password_ = Scrypt::Hash(vec);
}

const UserGroupMembershipSet &User::group_memberships() const
{
    return membership_;
}

bool User::verify_password(const std::string &pw) const
{
    std::vector<uint8_t> vec(pw.begin(), pw.end());
    return Scrypt::Verify(vec, password_);
}
