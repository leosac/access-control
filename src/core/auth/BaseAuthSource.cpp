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

#include <tools/IVisitor.hpp>
#include "BaseAuthSource.hpp"

using namespace Leosac::Auth;

BaseAuthSource::BaseAuthSource(const std::string &cred_id)
    : id_(cred_id)
{
}

void BaseAuthSource::addAuthSource(IAuthenticationSourcePtr source)
{
    subsources_.push_back(source);
}

void BaseAuthSource::owner(IUserPtr user)
{
    owner_ = user;
}

IUserPtr BaseAuthSource::owner() const
{
    return owner_;
}

IAccessProfilePtr BaseAuthSource::profile() const
{
    return profile_;
}

void BaseAuthSource::accept(Leosac::Tools::IVisitor *visitor)
{
    visitor->visit(this);
}

const std::string &BaseAuthSource::name() const
{
    return source_name_;
}

void BaseAuthSource::name(std::string const &n)
{
    source_name_ = n;
}

std::string BaseAuthSource::to_string() const
{
    return "Base auth source";
}

const std::string &BaseAuthSource::id() const
{
    return id_;
}

void BaseAuthSource::id(const std::string &cred_id)
{
    id_ = cred_id;
}

const CredentialValidity &BaseAuthSource::validity() const
{
    return validity_;
}

void BaseAuthSource::validity(const CredentialValidity &c)
{
    validity_ = c;
}

void BaseAuthSource::profile(IAccessProfilePtr p)
{
    profile_ = p;
}
