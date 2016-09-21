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

#include "IAuditEntry.hpp"
#include "core/auth/AuthFwd.hpp"

namespace Leosac
{
namespace Audit
{
/**
 * Interface to audit object that take care of tracking
 * user/group membership change.
 */
class IUserGroupMembershipEvent : virtual public IAuditEntry
{
  public:
    /**
     * The user that joined or left the group.
     */
    virtual void target_user(Auth::UserPtr user) = 0;


    /**
     * The group that gained or lost an user.
     */
    virtual void target_group(Auth::GroupPtr grp) = 0;
};
}
}
