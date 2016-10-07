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

#include <cstddef>
#include <memory>
#include <odb/lazy-ptr.hxx>

namespace Leosac
{
namespace Auth
{
class User;
using UserPtr   = std::shared_ptr<User>;
using UserLPtr  = odb::lazy_shared_ptr<User>;
using UserLWPtr = odb::lazy_weak_ptr<User>;
using UserId    = unsigned long;

class Group;
using GroupPtr   = std::shared_ptr<Group>;
using GroupWPtr  = std::weak_ptr<Group>;
using GroupLWPtr = odb::lazy_weak_ptr<Group>;
using GroupLPtr  = odb::lazy_shared_ptr<Group>;
using GroupId    = unsigned long;

/**
 * The rank of an User inside a Group.
 */
enum class GroupRank
{
    MEMBER   = 0,
    OPERATOR = 1,
    ADMIN    = 2,
};

enum class UserRank
{
    /**
     * A default user.
     */
    USER = 0,
    /**
     * Has viewing permission
     */
    VIEWER = 1,
    /**
     * Can access user management aswel as credential management.
     */
    MANAGER = 2,
    /**
     * Full access except changing user role.
     */
    SUPERVISOR = 3,
    /**
     * Site administrator.
     */
    ADMIN = 4,
};

class UserGroupMembership;
using UserGroupMembershipPtr = std::shared_ptr<UserGroupMembership>;
using UserGroupMembershipId  = unsigned long;

class Token;
using TokenPtr = std::shared_ptr<Token>;

class IAccessProfile;
using IAccessProfilePtr = std::shared_ptr<IAccessProfile>;

class ValidityInfo;

class AuthTarget;
using AuthTargetPtr = std::shared_ptr<AuthTarget>;

class IAuthenticationSource;
using IAuthenticationSourcePtr = std::shared_ptr<IAuthenticationSource>;

class BaseAuthSource;
using BaseAuthSourcePtr = std::shared_ptr<BaseAuthSource>;

class WiegandCard;
}
}
