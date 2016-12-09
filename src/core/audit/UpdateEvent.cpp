/*
    Copyright (C) 2014-2016 Leosac

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

#include "UpdateEvent.hpp"
#include "UpdateEvent_odb.h"
#include "tools/AssertCast.hpp"
#include "tools/db/OptionalTransaction.hpp"
#include "tools/log.hpp"

namespace Leosac
{
namespace Audit
{

std::shared_ptr<UpdateEvent> UpdateEvent::create(const DBPtr &database,
                                                 update::IUpdatePtr target_update,
                                                 AuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");
    ASSERT_LOG(target_update, "Target update must be non null.");
    ASSERT_LOG(target_update->id(), "Target update must be already persisted.");
    ASSERT_LOG(parent, "Parent must be non null.");
    ASSERT_LOG(parent->id(), "Parent must be already persisted.");

    db::OptionalTransaction t(database->begin());

    UpdateEventPtr audit(new UpdateEvent());
    audit->database_ = database;
    audit->target(target_update);
    database->persist(audit);

    audit->set_parent(parent);
    database->update(audit);

    t.commit();
    return audit;
}

void UpdateEvent::target(update::IUpdatePtr u)
{
    auto update_odb = assert_cast<update::UpdatePtr>(u);
    target_         = update_odb;
}

update::UpdateId UpdateEvent::target_id() const
{
    if (target_.lock())
        return target_.object_id();
    return 0;
}

std::shared_ptr<UpdateEvent> UpdateEvent::create_empty()
{
    return UpdateEventPtr(new UpdateEvent());
}
}
}
