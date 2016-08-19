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

#pragma once

#include "core/auth/Interfaces/IAccessProfile.hpp"
#include "core/auth/AuthFwd.hpp"
#include "core/auth/CredentialValidity.hpp"
#include "tools/db/database.hpp"
#include <memory>

namespace Leosac
{
namespace Auth
{
class IUser;
using IUserPtr = std::shared_ptr<IUser>;

/**
* Represent a user
*/
#pragma db object pointer(std::shared_ptr)
class User
{
  public:
    User(const std::string &username);
    User()          = default;
    virtual ~User() = default;

    /**
    * Get the username of this user.
    */
    const std::string &username() const noexcept;

    unsigned long id() const noexcept;

    /**
     * Retrieve password
     */
    const std::string &password() const noexcept;

    /**
    * Set a new username.
    */
    void username(const std::string &username);

    IAccessProfilePtr profile() const noexcept;

    void profile(IAccessProfilePtr user_profile);

    const std::string &firstname() const;
    const std::string &lastname() const;
    const std::string &email() const;
    const CredentialValidity &validity() const;

    void firstname(const std::string &);
    void lastname(const std::string &);
    void email(const std::string &);
    void validity(const CredentialValidity &c);

    /**
    * Check the credentials validity object.
    */
    bool is_valid() const;

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

#pragma db not_null
    std::string password_;

    std::string firstname_;
    std::string lastname_;
    std::string email_;

/**
* A user can have the same validity than credentials.
*/
#pragma db transient
    CredentialValidity validity_;
#pragma db transient
    IAccessProfilePtr profile_;

  private:
    friend class odb::access;
};
}
}
