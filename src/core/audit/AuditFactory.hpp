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

#include "core/audit/AuditFwd.hpp"
#include "core/auth/AuthFwd.hpp"
#include "tools/db/db_fwd.hpp"

namespace Leosac
{
namespace Audit
{
/**
 * Provide static methods to instanciate various Audit objects.
 *
 * This is the class to use to instanciate Audit object. All other means to
 * construct an instance are private to the class. This Factory is friend
 * with audit objects providing the implementation of the various Audit interfaces.
 *
 * Basically, methods in this class forward to private factory function in each
 * Audit object.
 *
 * @note The naming convention differs for those methods: each method is named
 * after the type of audit object it instanciates.
 */
class Factory
{
  public:
    static IUserEventPtr UserEvent(const DBPtr &database, Auth::UserPtr target_user,
                                   IAuditEntryPtr parent);

    static IGroupEventPtr GroupEvent(const DBPtr &database,
                                     Auth::GroupPtr target_group,
                                     IAuditEntryPtr parent);

    static IWSAPICallPtr WSAPICall(const DBPtr &database);


    static IUserGroupMembershipEventPtr
    UserGroupMembershipEvent(const DBPtr &database, Auth::GroupPtr target_group,
                             Auth::UserPtr target_user, IAuditEntryPtr parent);
};
}
}
