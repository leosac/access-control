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

#include <memory>
#include <json.hpp>
#include <sys/epoll.h>

namespace Leosac
{
namespace Module
{
namespace WebSockAPI
{

class WSServer;

/**
 * This is the application-level object that provide
 * the API.
 *
 * One objected is instantiated per websocket client.
 */
class API {
      public:
        using json = nlohmann::json;

        /**
         * Enumeration describing the authentication status of a client.
         */
        enum class AuthStatus
        {
                NONE,
                LOGGED_IN
        };

        API(WSServer &server);
        API(const API &) = delete;
        API(API &&) = delete;

        /**
         * Retrieve the current version number of Leosac.
         */
        json get_leosac_version(const json &);

        /**
         * Generate an authentication token and log the user in.
         *
         * The request is excepted to contain credential.
         */
        json get_auth_token(const json &req);

        /**
         * Attempt to authenticate with an authentication token.
         */
        json authenticate_with_token(const json &req);

        /**
         * Log an user out.
         */
        json logout(const json &req);

      private:
        /**
         * The API server.
         */
        WSServer &server_;
        AuthStatus auth_status_;

        /**
         * The token we are authenticated with.
         */
        std::string current_auth_token_;
};

using APIPtr = std::shared_ptr<API>;

}
}
}
