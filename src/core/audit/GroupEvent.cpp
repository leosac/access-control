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

#include "GroupEvent.hpp"
#include "GroupEvent_odb.h"
#include "core/auth/Group.hpp"
#include "tools/db/OptionalTransaction.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Audit;

std::shared_ptr<GroupEvent> GroupEvent::create(const DBPtr &database,
                                               Auth::GroupPtr target_group,
                                               AuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");
    ASSERT_LOG(target_group, "Target group must be non null.");
    ASSERT_LOG(target_group->id(), "Target group must be already persisted.");
    ASSERT_LOG(parent, "Parent must be non null.");
    ASSERT_LOG(parent->id(), "Parent must be already persisted.");

    db::OptionalTransaction t(database->begin());

    Audit::GroupEventPtr audit =
        std::shared_ptr<Audit::GroupEvent>(new Audit::GroupEvent());
    audit->database_ = database;
    audit->target_   = target_group;
    database->persist(audit);

    audit->set_parent(parent);
    database->update(audit);

    t.commit();
    return audit;
}

void GroupEvent::target(Auth::GroupPtr user)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    target_ = user;
}

void GroupEvent::before(const std::string &repr)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    before_ = repr;
}

void GroupEvent::after(const std::string &repr)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    after_ = repr;
}
