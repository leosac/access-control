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

#include "DBService.hpp"
#include "Credential_odb.h"
#include "DatabaseTracer.hpp"
#include "Group_odb.h"
#include "OptionalTransaction.hpp"
#include "Schedule_odb.h"
#include "User_odb.h"
#include "exception/EntityNotFound.hpp"
#include "tools/log.hpp"
#include <odb/database.hxx>

using namespace Leosac;


DBService::DBService(DBPtr db)
    : database_(db)
{
}

DBPtr DBService::db() const
{
    return database_;
}

size_t DBService::operation_count() const
{
    if (database_->tracer())
    {
        auto tracer = dynamic_cast<db::DatabaseTracer *>(database_->tracer());
        ASSERT_LOG(tracer, "No (valid?) tracer object on the database.");
        return tracer->count();
    }
    return 0;
}

Auth::GroupPtr DBService::find_group_by_id(const Auth::GroupId &id, Flag flags)
{
    db::OptionalTransaction t(database_->begin());
    auto grp = database_->find<Auth::Group>(id);
    t.commit();
    if (!grp && flags & Flag::THROW_IF_NOT_FOUND)
        throw EntityNotFound(id, "group");
    return grp;
}

Auth::UserPtr DBService::find_user_by_id(const Auth::UserId &id, Flag flags)
{
    db::OptionalTransaction t(database_->begin());
    auto user = database_->find<Auth::User>(id);
    t.commit();
    if (!user && flags & Flag::THROW_IF_NOT_FOUND)
        throw EntityNotFound(id, "user");
    return user;
}

Auth::UserGroupMembershipPtr
DBService::find_membership_by_id(const Auth::UserGroupMembershipId &id, Flag flags)
{
    db::OptionalTransaction t(database_->begin());
    auto ugm = database_->find<Auth::UserGroupMembership>(id);
    t.commit();
    if (!ugm && flags & Flag::THROW_IF_NOT_FOUND)
        throw EntityNotFound(id, "user-group-membership");
    return ugm;
}

Cred::ICredentialPtr DBService::find_credential_by_id(const Cred::CredentialId &id,
                                                      DBService::Flag flags)
{
    db::OptionalTransaction t(database_->begin());
    auto cred = database_->find<Cred::Credential>(id);
    t.commit();
    if (!cred && flags & Flag::THROW_IF_NOT_FOUND)
        throw EntityNotFound(id, "credential");
    return cred;
}

Tools::ISchedulePtr DBService::find_schedule_by_id(const Tools::ScheduleId &id,
                                                   DBService::Flag flags)
{
    db::OptionalTransaction t(database_->begin());
    auto sched = database_->find<Tools::Schedule>(id);
    t.commit();
    if (!sched && flags & Flag::THROW_IF_NOT_FOUND)
        throw EntityNotFound(id, "schedule");
    return sched;
}
