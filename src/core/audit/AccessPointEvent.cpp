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

#include "AccessPointEvent.hpp"
#include "LeosacFwd.hpp"
#include "core/audit/AccessPointEvent_odb.h"
#include "core/auth/AccessPoint.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/db/OptionalTransaction.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Audit;

AccessPointEvent::AccessPointEvent()
    : target_ap_id_(0)
{
}

std::shared_ptr<AccessPointEvent>
AccessPointEvent::create(const DBPtr &database, Auth::IAccessPointPtr target_ap,
                         AuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");
    ASSERT_LOG(target_ap, "Target AccessPoint must be non null.");
    ASSERT_LOG(target_ap->id(), "Target AccessPoint must be already persisted.");
    ASSERT_LOG(parent, "Parent must be non null.");
    ASSERT_LOG(parent->id(), "Parent must be already persisted.");

    db::OptionalTransaction t(database->begin());

    Audit::AccessPointEventPtr audit =
        std::shared_ptr<Audit::AccessPointEvent>(new Audit::AccessPointEvent());
    audit->database_ = database;
    audit->target(target_ap);
    database->persist(audit);

    audit->set_parent(parent);
    database->update(audit);

    t.commit();
    return audit;
}

void AccessPointEvent::target(Auth::IAccessPointPtr ap)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    if (ap)
        ASSERT_LOG(ap->id(), "AccessPoint has no id.");
    auto ap_odb = std::dynamic_pointer_cast<Auth::AccessPoint>(ap);
    ASSERT_LOG(ap_odb, "IAccessPoint is not of type AccessPoint.");

    target_       = ap_odb;
    target_ap_id_ = ap->id();
}

void AccessPointEvent::before(const std::string &repr)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    before_ = repr;
}

void AccessPointEvent::after(const std::string &repr)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    after_ = repr;
}

Auth::AccessPointId AccessPointEvent::target_id() const
{
    if (target_.lock())
        return target_.object_id();
    return target_ap_id_;
}

const std::string &AccessPointEvent::before() const
{
    return before_;
}

const std::string &AccessPointEvent::after() const
{
    return after_;
}

std::string AccessPointEvent::generate_description() const
{
    std::stringstream ss;

    if (event_mask_ & EventType::ACCESS_POINT_CREATED)
        ss << "AccessPoint " << generate_target_description()
           << " has been created.";
    else if (event_mask_ & EventType::ACCESS_POINT_UPDATED)
        ss << "AccessPoint " << generate_target_description() << " has been edited.";
    else if (event_mask_ & EventType::ACCESS_POINT_DELETED)
        ss << "AccessPoint " << generate_target_description()
           << " has been deleted.";

    return ss.str();
}

std::string AccessPointEvent::generate_target_description() const
{
    Leosac::json desc;

    desc["id"] = target_id();
    auto t     = target_.load();
    if (t)
        desc["alias"] = t->alias();

    return desc.dump();
}

AccessPointEventPtr AccessPointEvent::create_empty()
{
    return AccessPointEventPtr(new AccessPointEvent());
}
