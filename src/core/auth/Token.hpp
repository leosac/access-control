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

#include "core/auth/AuthFwd.hpp"
#include "exception/leosacexception.hpp"
#include "tools/db/database.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <chrono>

namespace Leosac
{
namespace Auth
{
/**
 * An authentication token used for authenticating a user against
 * Leosac.
 *
 * The authentication token can be persisted in database. This lets
 * user authenticate with the same token between reboot.
 *
 * One token maps to one user.
 */
#pragma db object
class Token
{
  public:
    Token() = default;

    Token(const std::string &token, UserPtr owner)
        : token_(token)
        , owner_(owner)
        , version_(0)
    {
        expire_in(std::chrono::milliseconds(0));
    }

    /**
     * Retrieve the string representation of the token.
     * @return
     */
    const std::string &token() const;

    /**
     * Check if the token is still active.
     */
    bool is_valid() const;

    /**
     * Retrieve a shared_ptr to the user owning
     * the token.
     */
    UserPtr owner() const;

    /**
     * Retrieve the unix timestamp at which the token
     * will/has expire(d).
     */
    boost::posix_time::ptime expiration() const;

    /**
     * Set the expiration point of the token to be `now + duration`.
     */
    template <typename T>
    void expire_in(const T &duration)
    {
        using namespace std::chrono;
        auto duration_second = duration_cast<seconds>(duration);

        auto now    = boost::posix_time::second_clock::local_time();
        expiration_ = now + boost::posix_time::seconds(duration_second.count());
        version_++;
    }

  private:
    friend class odb::access;

/**
 * The string representation of the token.
 */
#pragma db id
#pragma db type("VARCHAR(128)")
    std::string token_;

/**
 * The user owning the token.
 */
#pragma db not_null
    UserPtr owner_;

#pragma db not_null
    boost::posix_time::ptime expiration_;

/**
 * The version indicates how many time the expiration
 * date has been modified.
 */
#pragma db not_null
    ssize_t version_;
};
}
}

#ifdef ODB_COMPILER
#include "core/auth/User.hpp"
#endif
