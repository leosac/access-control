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

#include "core/credentials/Credential.hpp"
#include "User_odb.h"
#include "core/credentials/CredentialValidator.hpp"

using namespace Leosac;
using namespace Leosac::Cred;

Credential::Credential()
    : id_(0)
{
}

Auth::UserLPtr Credential::owner() const
{
    return owner_;
}

void Credential::owner(Auth::UserLPtr ptr)
{
    owner_ = ptr;
}

std::string Credential::alias() const
{
    return alias_;
}

CredentialId Credential::id() const
{
    return id_;
}

size_t Credential::odb_version() const
{
    return odb_version_;
}

Auth::UserId Credential::owner_id() const
{
    if (owner_)
        return owner_.object_id();
    return 0;
}

void Credential::alias(const std::string &alias)
{
    CredentialValidator::validate_alias(alias);
    alias_ = alias;
}

std::string Credential::description() const
{
    return description_;
}

void Credential::description(const std::string &str)
{
    description_ = str;
}

void Credential::validity(const Auth::ValidityInfo &info)
{
    validity_ = info;
}

const Auth::ValidityInfo &Credential::validity() const
{
    return validity_;
}

std::vector<Tools::ScheduleMappingLWPtr> Credential::lazy_schedules_mapping() const
{
    return schedules_mapping_;
}
