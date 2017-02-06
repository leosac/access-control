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

#include "core/SecurityContext.hpp"
#include "core/auth/AuthFwd.hpp"
#include <json.hpp>
#include <memory>

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
class APISession
{
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

    APISession(WSServer &server);
    APISession(const APISession &) = delete;
    APISession(APISession &&)      = delete;

    /**
     * Retrieve the UserId of the user associated with this API session.
     * @return User Id or 0 if the session is not authenticated.
     */
    Auth::UserId current_user_id() const;

    /**
     * Retrieve the user associated with the session, or nullptr.
     * @return Pointer to current user, or nullptr.
     */
    Auth::UserPtr current_user() const;

    /**
     * Retrieve the currently in-use token, or nullptr.
     */
    Auth::TokenPtr current_token() const;

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
     * Generate an authentication token using the user credential,
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
     *     + `user_id`: On success, the identifier of the logged in user.
     *     + `token`: On success, value of the generated authentication token.
     *     + `message`: An optional text message describing the status.
     */
    json create_auth_token(const json &req);

    /**
     * Attempt to authenticate with a (previously generated) authentication token.
     *
     * Request:
     *     + `token`: The authentication token
     *
     * Response:
     *     + `user_id`: On success, the user-id of the newly authenticated user.
     *     + `username`: On success, the username authenticated user.
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
     * A hook that is called before a request processing method
     * will be invoked.
     *
     * Typically, this is used to:
     *     + Check that the authentication is still valid.
     *     + Extend the authentication duration.
     */
    void hook_before_request();

    /**
     * Abort the current websocket session.
     *
     * This means that we reset the authentication status.
     */
    void abort_session();

    SecurityContext &security_context() const;

  private:
    void mark_authenticated(Auth::TokenPtr token);
    void clear_authentication();

    /**
     * The API server.
     */
    WSServer &server_;
    AuthStatus auth_status_;

    /**
     * The token we are authenticated with.
     */
    Auth::TokenPtr current_auth_token_;

    std::unique_ptr<SecurityContext> security_;
};
}
}
}
