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
     * Marks the `token` has invalid, effectively removing it
     * from the list of valid tokens.
     */
    void invalidate_token(Auth::TokenPtr token) const;

  private:
    /**
     * Map a token to a user identifier.
     */
    using TokenMap = std::map<std::string, ::Leosac::Auth::UserId>;
    TokenMap tokens_;

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
