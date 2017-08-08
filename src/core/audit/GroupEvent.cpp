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

#include "GroupEvent.hpp"
#include "core/audit/GroupEvent_odb.h"
#include "core/auth/Group.hpp"
#include "tools/JSONUtils.hpp"
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
    audit->target(target_group);
    database->persist(audit);

    audit->set_parent(parent);
    database->update(audit);

    t.commit();
    return audit;
}

void GroupEvent::target(Auth::GroupPtr grp)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    if (grp)
        ASSERT_LOG(grp->id(), "Group has no id.");
    target_          = grp;
    target_group_id_ = grp->id();
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

Auth::GroupId GroupEvent::target_id() const
{
    if (target_.lock())
    {
        return target_.object_id();
    }
    return target_group_id_;
}

const std::string &GroupEvent::before() const
{
    return before_;
}

const std::string &GroupEvent::after() const
{
    return after_;
}

std::string GroupEvent::generate_description() const
{
    using namespace FlagSetOperator;
    std::stringstream ss;

    if (event_mask_ & Audit::EventType::GROUP_CREATED)
        ss << "Group " << generate_target_description() << " has been created.";
    else if (event_mask_ & Audit::EventType::GROUP_UPDATED)
        ss << "Group " << generate_target_description() << " has been edited.";
    else if (event_mask_ & Audit::EventType::GROUP_DELETED)
        ss << "Group " << generate_target_description() << " has been deleted.";

    return ss.str();
}

std::string GroupEvent::generate_target_description() const
{
    Leosac::json desc;

    desc["id"] = target_id();
    auto t     = target_.load();
    if (t)
        desc["name"] = t->name();

    return desc.dump();
}

std::shared_ptr<GroupEvent> GroupEvent::create_empty()
{
    return GroupEventPtr(new GroupEvent());
}
