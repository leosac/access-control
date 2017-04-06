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

#include "LeosacFwd.hpp"
#include "core/auth/AuthFwd.hpp"
#include "core/auth/Interfaces/IAccessProfile.hpp"
#include "core/auth/UserGroupMembership.hpp"
#include "core/auth/ValidityInfo.hpp"
#include "core/credentials/CredentialFwd.hpp"
#include "tools/db/database.hpp"
#include "tools/scrypt/Scrypt.hpp"
#include <boost/optional.hpp>
#include <memory>

namespace Leosac
{
namespace Auth
{

/**
* Represent a user
*/
#pragma db object optimistic
class User : public std::enable_shared_from_this<User>
{
  public:
    /**
     * Create a user, specifying an username.
     *
     * @see `username(const std::string &)` for more information regarding
     * username's rules.
     */
    User(const std::string &username);
    User();

    /**
     * Create a user with a predefined UserId.
     * @param id
     */
    User(const UserId &id);

    virtual ~User() = default;

    /**
    * Get the username of this user.
    */
    const std::string &username() const noexcept;

    UserId id() const noexcept;

    /**
     * Set a new password for the user.
     * @param `pw` A cleartext representation of the password.
     */
    void password(const std::string &pw);

    /**
     * Verify that the password `pw` is equal to the user's password.
     *
     * @param pw A cleartext string representing the password you want to check.
     * @return True is `pw` is the correct password, false otherwise.
     */
    bool verify_password(const std::string &pw) const;

    /**
     * Returns the password hash + salt (as stored in the database).
     *
     * The use of this function should be rather sparse.
     */
    std::string password() const;

    /**
     * Set a new username.
     *
     * A few rules regarding usernames:
     *     + Username are always case insensitive, and are store in lower case.
     *     + Only alphanumeric ASCII characters, "_", "." and "-" are allowed.
     *
     * @note This function will throw if the username is invalid.
     */
    void username(const std::string &username);

    IAccessProfilePtr profile() const noexcept;

    void profile(IAccessProfilePtr user_profile);

    const std::string &firstname() const;
    const std::string &lastname() const;
    const std::string &email() const;
    const ValidityInfo &validity() const;

    void firstname(const std::string &);
    void lastname(const std::string &);
    void email(const std::string &);
    void validity(const ValidityInfo &c);

    /**
    * Check the validity status (enabled / disabled) of the user.
    */
    bool is_valid() const;

    /**
     * Retrieve the UserGroupMembership that this user is
     * involved with.
     *
     * While the set is always eagerly loaded, the `group()` and `user()`
     * method in each Membership will return lazy weak pointer.
     */
    const UserGroupMembershipSet &group_memberships() const;

    /**
     * Get the global rank of the user.
     */
    UserRank rank() const;

    /**
     * Set the global rank of the user.
     */
    void rank(UserRank r);

    size_t odb_version() const;

    void add_credential(const Cred::ICredentialPtr &cred);

    std::vector<Cred::CredentialLWPtr> lazy_credentials() const;

    std::vector<Tools::ScheduleMappingLWPtr> lazy_schedules_mapping() const;

    /**
     * The user has been mapped by a schedule.
     * @see Auth::Door::schedule_mapping_added
     */
    void schedule_mapping_added(const Tools::ScheduleMappingPtr &sched_mapping);

  protected:
#pragma db id auto
    UserId id_;

/**
* This is an (unique) identifier for the user.
*/
#pragma db unique
#pragma db not_null
#pragma db type("VARCHAR(128)")
    std::string username_;

#pragma db type("TEXT")
    boost::optional<ScryptResult> password_;

    std::string firstname_;
    std::string lastname_;
    std::string email_;

#pragma db value_not_null inverse(user_)
    UserGroupMembershipSet membership_;

#pragma db not_null
    UserRank rank_;

#pragma db value_not_null inverse(owner_)
    std::vector<Cred::CredentialLWPtr> credentials_;

/**
 * ScheduleMapping object to which we are mapped directly (as user).
 */
#pragma db value_not_null inverse(users_)
    std::vector<Tools::ScheduleMappingLWPtr> schedules_mapping_;

    /**
    * A user can have the same validity than credentials.
    */
    ValidityInfo validity_;
#pragma db transient
    IAccessProfilePtr profile_;

#pragma db version
    const size_t version_;

  private:
    friend class odb::access;
    friend class ::Leosac::TestAccess;
};
}
}

#ifdef ODB_COMPILER
#include "core/auth/Group.hpp"
#include "core/auth/UserGroupMembership.hpp"
#include "core/credentials/Credential.hpp"
#include "tools/ScheduleMapping.hpp"
#endif
