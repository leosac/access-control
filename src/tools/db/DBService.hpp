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

#pragma once

#include "core/audit/IAuditEntry.hpp"
#include "core/auth/AuthFwd.hpp"
#include "core/credentials/CredentialFwd.hpp"
#include "tools/ToolsFwd.hpp"
#include "tools/db/db_fwd.hpp"

namespace Leosac
{
/**
 * Provides various database-related services to consumer.
 */
class DBService
{
  public:
    enum Flag
    {
        DEFAULT            = 0,
        THROW_IF_NOT_FOUND = 1
    };
    DBService(DBPtr db);

    /**
     * Simply returns the underlying database pointer.
     */
    DBPtr db() const;

    /**
     * Return the number of operation against the database.
     */
    size_t operation_count() const;

    /**
     * Retrieve a group by its id.
     *
     * @return The group, or nullptr.
     */
    Auth::GroupPtr find_group_by_id(const Auth::GroupId &id, Flag f = Flag::DEFAULT);

    /**
     * Find a user by its id.
     */
    Auth::UserPtr find_user_by_id(const Auth::UserId &id, Flag f = Flag::DEFAULT);

    Auth::UserGroupMembershipPtr
    find_membership_by_id(const Auth::UserGroupMembershipId &id,
                          Flag f = Flag::DEFAULT);

    Cred::ICredentialPtr find_credential_by_id(const Cred::CredentialId &id,
                                               Flag f = Flag::DEFAULT);

    Tools::ISchedulePtr find_schedule_by_id(const Tools::ScheduleId &id,
                                            Flag f = Flag::DEFAULT);

    Auth::IDoorPtr find_door_by_id(const Auth::DoorId &id, Flag f = Flag::DEFAULT);

    Audit::IAuditEntryPtr find_audit_by_id(const Audit::AuditEntryId &id,
                                           Flag f = Flag::DEFAULT);

    Auth::IAccessPointPtr find_access_point_by_id(const Auth::AccessPointId &id,
                                                  Flag f = Flag::DEFAULT);

    /**
     * Persist an audit entry object.
     *
     * Begin and commit a new transaction if there is currently no
     * transaction in progress. Otherwise, simple persist the object
     * in the current transaction, but does not call commit().
     *
     * @note The object may not be const because its using an automatically assigned
     *       database id.
     */
    void persist(Audit::IAuditEntry &);

    /**
     * Update the object.
     */
    void update(Audit::IAuditEntry &);

  private:
    const DBPtr database_;
};
}
