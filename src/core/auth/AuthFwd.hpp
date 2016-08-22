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

#pragma once

#include <memory>

namespace Leosac
{
namespace Auth
{
class User;
using UserPtr = std::shared_ptr<User>;
using UserId  = unsigned long;

class Group;
using GroupPtr  = std::shared_ptr<Group>;
using GroupWPtr = std::weak_ptr<Group>;
using GroupId   = unsigned long;

class IAccessProfile;
using IAccessProfilePtr = std::shared_ptr<IAccessProfile>;

class CredentialValidity;

class AuthTarget;
using AuthTargetPtr = std::shared_ptr<AuthTarget>;

class IAuthenticationSource;
using IAuthenticationSourcePtr = std::shared_ptr<IAuthenticationSource>;

class BaseAuthSource;
using BaseAuthSourcePtr = std::shared_ptr<BaseAuthSource>;

class WiegandCard;
}
}
