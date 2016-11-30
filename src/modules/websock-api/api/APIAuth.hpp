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

#include "core/auth/AuthFwd.hpp"
#include "core/auth/Token.hpp"
#include <map>
#include <string>
#include <vector>

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
class WSServer;
/**
 * This class is responsible for providing an API to manage
 * authentication for Websocket client.
 *
 * The object is instantiated for the lifetime of the WSServer object.
 */
class APIAuth
{
  public:
    APIAuth(WSServer &srv);

    /**
     * Attempt to authenticate with username/password credential
     * and generate an authentication token.
     *
     * On success return a new authentication token that will
     * be valid when calling for further authentication.
     * On error returns nullptr.
     *
     * @note Username is case isensitive and will be converted to
     * lower case.
     */
    Auth::TokenPtr authenticate_credentials(const std::string &username,
                                            const std::string &password) const;

    /**
     * Attempt to authenticate with an authentication token.
     *
     * Authenticating with a valid token will update the expiration
     * date of the token.
     *
     * @param token The token string used to perform the authentication.
     * Returns the token object matching the token string on success, or nullptr
     * on failure.
     */
    Auth::TokenPtr authenticate_token(const std::string &token_str) const;

    /**
     * Invalidate the authentication token, removing it from the database.
     */
    void invalidate_token(Auth::TokenPtr token) const;

  private:
    /**
     * Make sure the User `u` is authorized to log in. This means
     * that we check that their ValidityInfo is valid.
     */
    void enforce_user_enabled(const Auth::User &u) const;

    /**
     * Reference to the Websocket server.
     * The websocket server is guaranteed to
     * outlive the APIAuth object.
     */
    WSServer &server_;
};
}
}
}
