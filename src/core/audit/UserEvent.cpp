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

#include "UserEvent.hpp"
#include "UserEvent_odb.h"
#include "core/auth/User.hpp"
#include "tools/JSONUtils.hpp"
#include "tools/db/OptionalTransaction.hpp"
#include "tools/log.hpp"

using namespace Leosac;
using namespace Leosac::Audit;

std::shared_ptr<UserEvent> UserEvent::create(const DBPtr &database,
                                             Auth::UserPtr target_user,
                                             AuditEntryPtr parent)
{
    ASSERT_LOG(database, "Database cannot be null.");
    ASSERT_LOG(target_user, "Target user must be non null.");
    ASSERT_LOG(target_user->id(), "Target user must be already persisted.");
    ASSERT_LOG(parent, "Parent must be non null.");
    ASSERT_LOG(parent->id(), "Parent must be already persisted.");

    db::OptionalTransaction t(database->begin());
    Audit::UserEventPtr audit =
        std::shared_ptr<Audit::UserEvent>(new Audit::UserEvent());
    audit->database_ = database;
    audit->target_   = target_user;
    database->persist(audit);

    // Now that we are persisted, set parent.
    // This make sure that if something bad happens, the parent is not left
    // with an invalid foreign key for its children.
    audit->set_parent(parent);
    database->update(audit);

    t.commit();
    return audit;
}

void UserEvent::target(Auth::UserPtr user)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    target_ = user;
}

void UserEvent::before(const std::string &repr)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    before_ = repr;
}

void UserEvent::after(const std::string &repr)
{
    ASSERT_LOG(!finalized(), "Audit entry is already finalized.");
    after_ = repr;
}

Auth::UserId UserEvent::target_id() const
{
    // todo check that it cant bug due to null target
    if (target_.lock())
        return target_.object_id();
    return 0;
}

const std::string &UserEvent::before() const
{
    return before_;
}

const std::string &UserEvent::after() const
{
    return after_;
}

std::string UserEvent::generate_description() const
{
    using namespace FlagSetOperator;
    std::stringstream ss;

    if (event_mask_ & EventType::USER_CREATED)
        ss << "User " << generate_target_description() << " has been created.";
    else if (event_mask_ & EventType::USER_EDITED)
        ss << "User " << generate_target_description() << " has been edited.";
    else if (event_mask_ & EventType::USER_DELETED)
        ss << "User " << generate_target_description() << " has been deleted.";

    return ss.str();
}

std::string UserEvent::generate_target_description() const
{
    Leosac::json desc;

    desc["id"] = target_id();
    auto t     = target_.load();
    if (t)
        desc["username"] = t->username();
    return desc.dump();
}
