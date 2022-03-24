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

#pragma once

#include "core/auth/AuthFwd.hpp"
#include "core/credentials/CredentialFwd.hpp"
#include "tools/ToolsFwd.hpp"
#include <vector>

namespace Leosac
{
namespace Tools
{
/**
 * Represent one of the mapping of a schedule.
 *
 * This object is designed to be embedded into Schedule object.
 */
#pragma db object optimistic
struct ScheduleMapping : public std::enable_shared_from_this<ScheduleMapping>
{
    ScheduleMappingId id() const;

    const std::string &alias() const;
    void alias(const std::string &);

    /**
     * Retrieve the id of the owning schedule.
     */
    ScheduleId schedule_id() const;

    ScheduleLWPtr schedule() const;

    const std::vector<Auth::UserLWPtr> &users() const;
    const std::vector<Auth::GroupLWPtr> &groups() const;
    const std::vector<Cred::CredentialLWPtr> &credentials() const;
    const std::vector<Auth::DoorLWPtr> &doors() const;

    size_t odb_version() const;

    /**
     * Check if the user with id `uid` is directly part of the
     * mapping.
     */
    bool has_user(Auth::UserId) const;

    /**
     * Check if the group is directly mapping by the ScheduleMapping object.
     */
    bool has_group(Auth::GroupId) const;

    /**
     * Check if the credential is mapped by this ScheduleMapping.
     */
    bool has_cred(Cred::CredentialId) const;

    /**
     * Check if the door is mapped by this object.
     */
    bool has_door(Auth::DoorId) const;

    /**
     * Check wether the user is mapped, either directly or not, by
     * this ScheduleMapping object.
     *
     * Being mapped indirectly means that either at least one of the user's
     * groups or credential is mapped.
     */
    bool has_user_indirect(Auth::UserPtr) const;

    /**
     * Add a door to the mapping.
     *
     * This call properly notify the door object (if eager) that a
     * ScheduleMapping has added it into its mapping.
     */
    void add_door(const Auth::DoorLPtr &door);

    void clear_doors();

    /**
     * Add a user to the mapping.
     *
     * This call will notify the user object (when possible) that it has
     * been added to the mapping.
     */
    void add_user(const Auth::UserLPtr &user);

    void clear_users();

    /**
     * Add a group to the mapping.
     *
     * This call will notify the group object (when possible) that it has
     * been added to the mapping.
     */
    void add_group(const Auth::GroupLPtr &group);

    void clear_groups();

    /**
     * Add a credential to the mapping.
     *
     * This call will notify the credential object (when possible) that it has
     * been added to the mapping.
     */
    void add_credential(const Cred::CredentialLPtr &cred);

    void clear_credential();

#pragma db id auto
    ScheduleMappingId id_;

  protected:
#pragma db id_column("schedule_mapping_id") value_column("user_id")
#pragma db value_not_null unordered
#pragma db on_delete(cascade)
    std::vector<Auth::UserLWPtr> users_;

#pragma db id_column("schedule_mapping_id") value_column("group_id")
#pragma db value_not_null unordered
#pragma db on_delete(cascade)
    std::vector<Auth::GroupLWPtr> groups_;

#pragma db id_column("schedule_mapping_id") value_column("credential_id")
#pragma db value_not_null unordered
#pragma db on_delete(cascade)
    std::vector<Cred::CredentialLWPtr> creds_;

#pragma db id_column("schedule_mapping_id") value_column("door_id")
#pragma db value_not_null unordered
#pragma db on_delete(cascade)
    std::vector<Auth::DoorLWPtr> doors_;

/**
 * The schedule that owns the mapping.
 */
#pragma db not_null inverse(mapping_)
    ScheduleLWPtr schedule_;

    /**
     * A non unique, optional alias.
     */
    std::string alias_;

#pragma db version
    size_t odb_version_;

  private:
    friend class Schedule;
    friend class odb::access;
};
}
}

#ifdef ODB_COMPILER
#include "core/auth/Door.hpp"
#include "core/auth/Group.hpp"
#include "core/auth/User.hpp"
#include "core/credentials/Credential.hpp"
#include "tools/Schedule.hpp"
#endif
