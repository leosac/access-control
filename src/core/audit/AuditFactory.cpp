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

#include "core/audit/AuditFactory.hpp"
#include "core/audit/IUserEvent.hpp"
#include "core/audit/UserEvent.hpp"
#include "core/audit/WSAPICall.hpp"
#include "core/auth/User.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Audit;

IUserEventPtr Factory::UserEvent(const DBPtr &database, Auth::UserPtr target_user,
                                 IAuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");
    ASSERT_LOG(target_user, "Target user must be non null.");
    ASSERT_LOG(target_user->id(), "Target user must be already persisted.");
    ASSERT_LOG(parent, "Parent must be non null.");
    ASSERT_LOG(parent->id(), "Parent must be already persisted.");

    AuditEntryPtr parent_odb = std::dynamic_pointer_cast<AuditEntry>(parent);
    ASSERT_LOG(parent_odb, "Parent object was not an instance of AuditEntry");

    return Audit::UserEvent::create(database, target_user, parent_odb);
}

IWSAPICallPtr Factory::WSAPICall(const DBPtr &database)
{
    ASSERT_LOG(database, "Database cannot be null.");

    return Audit::WSAPICall::create(database);
}
