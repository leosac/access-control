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

#include "DoorEvent.hpp"
#include "DoorEvent_odb.h"
#include "LeosacFwd.hpp"
#include "core/auth/Door.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/db/OptionalTransaction.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Audit;

DoorEvent::DoorEvent()
    : target_door_id_(0)
{
}

std::shared_ptr<DoorEvent> DoorEvent::create(const DBPtr &database,
                                             Auth::IDoorPtr target_door,
                                             AuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");
    ASSERT_LOG(target_door, "Target door must be non null.");
    ASSERT_LOG(target_door->id(), "Target door must be already persisted.");
    ASSERT_LOG(parent, "Parent must be non null.");
    ASSERT_LOG(parent->id(), "Parent must be already persisted.");

    db::OptionalTransaction t(database->begin());

    Audit::DoorEventPtr audit =
        std::shared_ptr<Audit::DoorEvent>(new Audit::DoorEvent());
    audit->database_ = database;
    audit->target(target_door);
    database->persist(audit);

    audit->set_parent(parent);
    database->update(audit);

    t.commit();
    return audit;
}

void DoorEvent::target(Auth::IDoorPtr door)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    if (door)
        ASSERT_LOG(door->id(), "Door has no id.");
    auto door_odb = std::dynamic_pointer_cast<Auth::Door>(door);
    ASSERT_LOG(door_odb, "IDoor is not of type Door.");

    target_         = door_odb;
    target_door_id_ = door->id();
}

void DoorEvent::before(const std::string &repr)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    before_ = repr;
}

void DoorEvent::after(const std::string &repr)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    after_ = repr;
}

Auth::DoorId DoorEvent::target_id() const
{
    if (target_.lock())
        return target_.object_id();
    return target_door_id_;
}

const std::string &DoorEvent::before() const
{
    return before_;
}

const std::string &DoorEvent::after() const
{
    return after_;
}

std::string DoorEvent::generate_description() const
{
    std::stringstream ss;

    if (event_mask_ & EventType::DOOR_CREATED)
        ss << "Door " << generate_target_description() << " has been created.";
    else if (event_mask_ & EventType::DOOR_UPDATED)
        ss << "Door " << generate_target_description() << " has been edited.";
    else if (event_mask_ & EventType::DOOR_DELETED)
        ss << "Door " << generate_target_description() << " has been deleted.";

    return ss.str();
}

std::string DoorEvent::generate_target_description() const
{
    Leosac::json desc;

    desc["id"] = target_id();
    auto t     = target_.load();
    if (t)
        desc["alias"] = t->alias();

    return desc.dump();
}

std::shared_ptr<DoorEvent> DoorEvent::create_empty()
{
    return std::shared_ptr<DoorEvent>(new DoorEvent());
}
