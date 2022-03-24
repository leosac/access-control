/*
    Copyright (C) 2014-2022 Leosac

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

#include "core/audit/ScheduleEvent.hpp"
#include "core/audit/ScheduleEvent_odb.h"
#include "tools/AssertCast.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/Schedule.hpp"
#include "tools/Schedule_odb.h"
#include "tools/db/OptionalTransaction.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Audit;

std::shared_ptr<ScheduleEvent>
ScheduleEvent::create(const DBPtr &database, Tools::ISchedulePtr target_sched,
                      AuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");
    ASSERT_LOG(target_sched, "Credential shall not be null.");
    ASSERT_LOG(target_sched->id(), "Credential must be already persisted.");

    db::OptionalTransaction t(database->begin());

    Audit::ScheduleEventPtr audit =
        std::shared_ptr<Audit::ScheduleEvent>(new Audit::ScheduleEvent());
    audit->database_ = database;
    audit->target(target_sched);
    database->persist(audit);

    audit->set_parent(parent);
    database->update(audit);

    t.commit();
    return audit;
}

std::shared_ptr<ScheduleEvent> ScheduleEvent::create_empty()
{
    return std::shared_ptr<ScheduleEvent>(new Audit::ScheduleEvent());
}

void ScheduleEvent::target(Tools::ISchedulePtr sched)
{
    ASSERT_LOG(sched, "sched must not be null.");
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    if (sched)
        ASSERT_LOG(sched->id(), "Schedule has no id.");

    auto sched_odb = assert_cast<Tools::SchedulePtr>(sched);

    target_          = sched_odb;
    target_sched_id_ = sched_odb->id();
}

void ScheduleEvent::before(const std::string &repr)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    before_ = repr;
}

void ScheduleEvent::after(const std::string &repr)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    after_ = repr;
}

Tools::ScheduleId ScheduleEvent::target_id() const
{
    if (target_.lock())
    {
        return target_.object_id();
    }
    return target_sched_id_;
}

const std::string &ScheduleEvent::before() const
{
    return before_;
}

const std::string &ScheduleEvent::after() const
{
    return after_;
}

std::string ScheduleEvent::generate_description() const
{
    using namespace FlagSetOperator;
    std::stringstream ss;

    auto target = target_.load();
    auto author = author_.load();

    if (event_mask_ & EventType::SCHEDULE_CREATED)
        ss << "Schedule " << generate_target_description() << " has been created.";
    else if (event_mask_ & EventType::SCHEDULE_UPDATED)
        ss << "Schedule " << generate_target_description() << " has been edited.";
    else if (event_mask_ & EventType::SCHEDULE_DELETED)
        ss << "Schedule " << generate_target_description() << " has been deleted.";

    return ss.str();
}

std::string ScheduleEvent::generate_target_description() const
{
    Leosac::json desc;

    desc["id"] = target_id();
    auto t     = target_.load();
    if (t)
        desc["name"] = t->name();

    return desc.dump();
}
