/*
    Copyright (C) 2014-2015 Islog

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

#include <string>
#include <map>
#include <vector>

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{
/**
 * This class is responsible for providing an API to manage
 * authentication for Websocket client.
 *
 * The object is instantiated for the lifetime of the WSServer object.
 */
class APIAuth
{
      public:
        /**
         * Attempt to authenticate with username/password credential
         * and generate an authentication token.
         *
         * On success return a new authentication token that will
         * be valid when calling `authenticate()`.
         * On error returns an empty string.
         */
        std::string generate_token(const std::string &username,
                                   const std::string &password);

        /**
         * Attempt to authenticate with an authentication token.
         *
         * @param token The token used to perform the authentication
         * @param user_id [out] The user_id who owns the token.
         * Returns `true` on success, `false` on failure.
         */
        bool authenticate(const std::string &token, std::string &user_id) const;

      private:
        /**
         * Map a token to a username
         */
        using TokenMap = std::map<std::string, std::string>;
        TokenMap tokens_;
};

}
}
}