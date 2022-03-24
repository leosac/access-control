/*
    Copyright (C) 2014-2022 Leosac

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

#include "core/auth/Group_odb.h"
#include "core/auth/User_odb.h"
#include "core/credentials/ICredential.hpp"
#include "exception/leosacexception.hpp"
#include "tools/AssertCast.hpp"
#include "tools/log.hpp"
#include <boost/algorithm/string.hpp>

using namespace Leosac::Auth;

static bool is_valid_username_character(char c)
{
    return isascii(c) && (isalnum(c) || c == '.' || c == '_' || c == '-');
}

User::User()
    : User(0)
{
}

User::User(const std::string &uname)
    : id_(0)
    , rank_(UserRank::USER)
    , version_(0)
{
    username(uname);
}

User::User(const UserId &id)
    : id_(id)
    , rank_(UserRank::USER)
    , version_(0)
{
}

const std::string &User::username() const noexcept
{
    for (const auto &c : username_)
        ASSERT_LOG(is_valid_username_character(c), "Invalid username.");
    return username_;
}

void User::username(const std::string &username)
{
    for (const auto &c : username)
    {
        if (is_valid_username_character(c))
            continue;
        throw LEOSACException("Invalid username: {" + username + "}");
    }
    username_ = boost::algorithm::to_lower_copy(username);
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

const ValidityInfo &User::validity() const
{
    return validity_;
}

void User::validity(const ValidityInfo &c)
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

const UserGroupMembershipSet &User::group_memberships() const
{
    return membership_;
}

void User::password(const std::string &pw)
{
    std::vector<uint8_t> vec(pw.begin(), pw.end());
    password_ = Scrypt::Hash(vec);
}

bool User::verify_password(const std::string &pw) const
{
    if (password_)
    {
        std::vector<uint8_t> vec(pw.begin(), pw.end());
        return Scrypt::Verify(vec, *password_);
    }
    return false;
}

std::string User::password() const
{
    if (password_)
        return std::string(password_->hash.begin(), password_->hash.end());
    return "";
}

UserRank User::rank() const
{
    return rank_;
}

void User::rank(UserRank r)
{
    rank_ = r;
}

size_t User::odb_version() const
{
    return version_;
}

std::vector<Leosac::Cred::CredentialLWPtr> User::lazy_credentials() const
{
    return credentials_;
}

std::vector<Leosac::Tools::ScheduleMappingLWPtr> User::lazy_schedules_mapping() const
{
    return schedules_mapping_;
}

void User::schedule_mapping_added(
    const Leosac::Tools::ScheduleMappingPtr &sched_mapping)
{
    schedules_mapping_.push_back(sched_mapping);
}

void User::add_credential(const Leosac::Cred::ICredentialPtr &cred)
{
    ASSERT_LOG(cred, "Credential is null.");
    ASSERT_LOG(cred->owner_id() == 0 || cred->owner_id() == id_,
               "Credential is already owned by someone else.");

    cred->owner(shared_from_this());
    credentials_.push_back(assert_cast<Cred::CredentialPtr>(cred));
}
