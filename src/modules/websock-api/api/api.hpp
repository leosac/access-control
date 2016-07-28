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
         * Is this API client allowed to perform the request `cmd` ?
         */
        bool allowed(const std::string &cmd);

        /**
         * Retrieve the current version number of Leosac.
         *
         * Request:
         *     + No parameter.
         *
         * Response:
         *     + `version`: An alphanumeric string representing the
         *     version of the Leosac daemon.
         */
        json get_leosac_version(const json &);

        /**
         * Generate an authentication token using the user crendetial,
         * and logs the user in on success.
         *
         * Request:
         *     + `username`: Username of the user attempting to log-in.
         *     + `password`: Clear text password.
         *
         * Response:
         *     + `status`: An integer.
         *         + `0` means success.
         *         + `-1` means invalid username/password.
         *         + `-2` means already logged in.
         *     + `message`: An optional text message describing the status.
         */
        json get_auth_token(const json &req);

        /**
         * Attempt to authenticate with a (previously generated) authentication token.
         *
         * Request:
         *     + `token`: The authentication token
         *
         * Response:
         *     + `user_id`: On success, the user-id of the newly authenticated user.
         *     + `status`: See [create_auth_token](@ref create_auth_token) status.
         *     + `message`: An optional text message describing the status.
         *
         * @note The auth token can be created using
         * the [create_auth_token](@ref create_auth_token) API call.
         */
        json authenticate_with_token(const json &req);

        /**
         * Log an user out.
         *
         * Request:
         *     + No parameter.
         *
         * Response:
         *     + No content.
         */
        json logout(const json &req);

        /**
         * Presents an overview of the system to the end user.
         *
         * Request:
         *     + No parameter
         *
         * Response:
         *     + `instance_name`: General config `instance_name` value.
         *     + `config_version`: The current version number of the configuration.
         *     + `uptime`: Number of seconds since Leosac started to run.
         *     + `modules`: List of name of the currently running modules.
         */
        json system_overview(const json &req);

        /**
         * Retrieve (part of) the logs generated Leosac. In order for this call
         * to work, SQLite logging must be enabled.
         *
         * Request:
         *     + `p`: The page number. Starts at 0.
         *     + `ps`: Page size: the number of item per page. Default to 20.
         *     + `sort`: Either 'asc' or 'desc'.
         *
         * Response:
         *     + ...
         */
        json get_logs(const json &req);

      private:

        /**
         * Extract the value of a key from a json object.
         *
         * If the key cannot be found, this function returns the default
         * value instead.
         */
        template<typename T>
        typename std::enable_if<!std::is_same<const char *, T>::value, T>::type
        extract_with_default(const json &obj, const std::string &key, T default_value)
        {
            T ret = default_value;
            try
        {
            ret = obj.at(key).get<T>();
        }
        catch (const std::out_of_range &e)
        {
        }
            return ret;
        }

        template<typename T>
        typename std::enable_if<std::is_same<const char *, T>::value, std::string>::type
        extract_with_default(const json &obj, const std::string &key, T default_value)
        {
            return extract_with_default<std::string>(obj, key, default_value);
        }

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
