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

#include "LeosacFwd.hpp"
#include "tools/db/DBService.hpp"
#include <zmqpp/zmqpp.hpp>
#include <memory>
#include <mutex>
#include <string>
#include <list>

namespace Leosac
{
namespace Module
{
namespace Auth 
{

struct AuthResult 
{
    AuthResult(bool s, ::Leosac::Auth::IAccessProfilePtr p,
               ::Leosac::Auth::UserPtr u) 
            : success(s)
            , profile(p)
            , user(u)
    {}

    /**
     * Access granted or denied
     */
    bool success;

    /**
     * Profile used to grant or deny access. May be null if no profiles
     * corresponding to the auth source were found.
     */
    ::Leosac::Auth::IAccessProfilePtr profile;

    /**
     * A user object representing the user who made the authentication
     * attempt.
     */
    ::Leosac::Auth::UserPtr user;
};

class AuthDBInstance : public std::enable_shared_from_this<AuthDBInstance>
{
    public:
        /**
        * Create an Authenticator instance that watches readers and writes authentication messages to the bus
        * @param ctx the ZeroMQ context
        * @param auth_ctx_name name of the instance in the auth_db module config
        * @param auth_source_names names of the authentication sources for the instance in the auth_db module config
        * @param auth_target_name name of the target attached to the instance
        * @param core_utils core utilities
        */
        AuthDBInstance(zmqpp::context &ctx, const std::string &auth_ctx_name,
                       const std::list<std::string> &auth_sources_names,
                       const std::string &auth_target_name,
                       CoreUtilsPtr core_utils);

        ~AuthDBInstance();

        AuthDBInstance(const AuthDBInstance &) = delete;
        AuthDBInstance &operator=(const AuthDBInstance &) = delete;

        /**
         * Handle a message from the bus
         */
        void handle_bus_msg();

    private:
        /**
         * Handle a message from the kernel
         */
        bool handle_kernel_msg(zmqpp::message &msg);

        /**
         * Handle a message from an auth source
         * Check if access is granted or denied by checking source against the database
         */
        AuthResult handle_auth(zmqpp::message *msg) noexcept;

        /**
         * Fetch credentials passed in message from database if they exist
         */
        Cred::ICredentialPtr find_db_credentials(zmqpp::message *msg);

        /**
         * Find credentials in database by card id and number of bits
         */
        Cred::ICredentialPtr find_credentials_by_card_id(const std::string &card_id, const int nb_bits) const;

        /**
         * Get the user from the db associated with the credentials
         */
         ::Leosac::Auth::UserPtr get_user(const Cred::ICredentialPtr *credentials);

        /**
         * Format auth result message
         */
        void format_auth_result_msg(zmqpp::message &msg);

        /**
         * Format user name for logging 
         */
        std::string format_user_name(const AuthResult &auth_result);

        /**
         * Append access result to auth result and log event to console
         */
        void update_and_log_auth_result_msg(const AuthResult &auth_result, zmqpp::message &auth_result_msg);

        /**
         * Database service to query 
        */
        std::shared_ptr<DBService> db_service_;

        /**
         * Socket to write authentication messages to the bus
        */
        zmqpp::socket bus_push_;

        /**
         * Socket to read authentication messages from the bus
        */
        zmqpp::socket bus_sub_;

        /**
         * Name of the auth context instance
        */
        std::string name_;

        /**
         * Name of the target to authenticate against
        */
        std::string target_name_;


        CoreUtilsPtr core_utils_;

        std::mutex mutex_;
};
}
}
}