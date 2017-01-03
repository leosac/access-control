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

#include "core/audit/ZoneEvent.hpp"
#include "LeosacFwd.hpp"
#include "ZoneEvent_odb.h"
#include "Zone_odb.h"
#include "core/auth/Zone.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/db/OptionalTransaction.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Audit;

ZoneEvent::ZoneEvent()
    : target_zone_id_(0)
{
}

std::shared_ptr<ZoneEvent> ZoneEvent::create(const DBPtr &database,
                                             Auth::IZonePtr target_zone,
                                             AuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");
    ASSERT_LOG(target_zone, "Target zone must be non null.");
    ASSERT_LOG(target_zone->id(), "Target zone must be already persisted.");
    ASSERT_LOG(parent, "Parent must be non null.");
    ASSERT_LOG(parent->id(), "Parent must be already persisted.");

    db::OptionalTransaction t(database->begin());

    Audit::ZoneEventPtr audit =
        std::shared_ptr<Audit::ZoneEvent>(new Audit::ZoneEvent());
    audit->database_ = database;
    audit->target(target_zone);
    database->persist(audit);

    audit->set_parent(parent);
    database->update(audit);

    t.commit();
    return audit;
}

void ZoneEvent::target(Auth::IZonePtr zone)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    if (zone)
        ASSERT_LOG(zone->id(), "Zone has no id.");
    auto zone_odb = std::dynamic_pointer_cast<Auth::Zone>(zone);
    ASSERT_LOG(zone_odb, "IZone is not of type Zone.");

    target_         = zone_odb;
    target_zone_id_ = zone->id();
}

void ZoneEvent::before(const std::string &repr)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    before_ = repr;
}

void ZoneEvent::after(const std::string &repr)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    after_ = repr;
}

Auth::ZoneId ZoneEvent::target_id() const
{
    if (target_.lock())
        return target_.object_id();
    return target_zone_id_;
}

const std::string &ZoneEvent::before() const
{
    return before_;
}

const std::string &ZoneEvent::after() const
{
    return after_;
}

std::string ZoneEvent::generate_description() const
{
    std::stringstream ss;

    if (event_mask_ & EventType::DOOR_CREATED)
        ss << "Zone " << generate_target_description() << " has been created.";
    else if (event_mask_ & EventType::DOOR_UPDATED)
        ss << "Zone " << generate_target_description() << " has been edited.";
    else if (event_mask_ & EventType::DOOR_DELETED)
        ss << "Zone " << generate_target_description() << " has been deleted.";

    return ss.str();
}

std::string ZoneEvent::generate_target_description() const
{
    Leosac::json desc;

    desc["id"] = target_id();
    auto t     = target_.load();
    if (t)
        desc["alias"] = t->alias();

    return desc.dump();
}

std::shared_ptr<ZoneEvent> ZoneEvent::create_empty()
{
    return std::shared_ptr<ZoneEvent>(new ZoneEvent());
}
