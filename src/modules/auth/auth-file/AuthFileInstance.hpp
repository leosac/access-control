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

#include "FileAuthSourceMapper.hpp"
#include "LeosacFwd.hpp"
#include "core/auth/AuthFwd.hpp"
#include "core/tasks/Task.hpp"
#include <fstream>
#include <zmqpp/zmqpp.hpp>

namespace Leosac
{
namespace Module
{
namespace Auth
{

class AuthFileInstance;
using AuthFileInstancePtr = std::shared_ptr<AuthFileInstance>;

struct AuthResult
{
    AuthResult(bool s, ::Leosac::Auth::IAccessProfilePtr p,
               ::Leosac::Auth::UserPtr u)
        : success(s)
        , profile(p)
        , user(u)
    {
    }

    /**
     * Wether access shall be granted, or not.
     */
    bool success;
    /**
     * Profile used to grant or deny access. May be null if no
     * corresponding to the auth source were found.
     */
    ::Leosac::Auth::IAccessProfilePtr profile;

    /**
     * A user object representing the user who made the authentication
     * attempt.
     */
    ::Leosac::Auth::UserPtr user;
};

/**
* An instance of an authentication handler that use files to check whether or not
* access is granted or denied.
* This class is for INTERNAL use only (by AuthFileModule).
*/
class AuthFileInstance : public std::enable_shared_from_this<AuthFileInstance>
{
  public:
    /**
    * Create a new Authenticator that watch a device and emit authentication message.
    * @param ctx the ZeroMQ context
    * @param auth_ctx_name name of this authentication context.
    * @param auth_sources_names names of the sources devices we watch (ie wiegand
    * reader).
    * @param auth_target_name name of the target (ie door) we auth against.
    * @param input_file path to file contain auth configuration
    * @param core_utils Core utilities
    */
    AuthFileInstance(zmqpp::context &ctx, const std::string &auth_ctx_name,
                     const std::list<std::string> &auth_sources_names,
                     const std::string &auth_target_name,
                     const std::string &input_file, CoreUtilsPtr core_utils);

    ~AuthFileInstance();

    AuthFileInstance(const AuthFileInstance &) = delete;

    AuthFileInstance &operator=(const AuthFileInstance &) = delete;

    /**
    * Something happened on the bus that we have interest into.
    */
    void handle_bus_msg();

    /**
    * Returns the socket subscribed to the message bus.
    */
    zmqpp::socket &bus_sub();

    /**
    * Return the name of the file associated with the authenticator.
    */
    const std::string &auth_file_name() const;

    /**
    * Return the content of the configuration file use for user/group and permission
    * mapping.
    */
    std::string auth_file_content() const;

  private:
    /**
     * Handle the message if its from Leosac's kernel, or
     * does nothing.
     *
     * Returns `true` if the message was handled.
     */
    bool handle_kernel_message(const zmqpp::message &msg);

    /**
     * Schedule an asynchronous reload of the module configuration file.
     */
    void reload_auth_config();

    /**
    * Prepare auth source object, map them to profile and check if access is granted.
    *
    * @note This is a `noexcept` method. Will return false in case something went
    * wrong.
    * @return A pair of boolean and profile. A `true` value for the boolean
    * means access shall be granted, otherwise it shall not.
    * The IAccessProfilePtr points to the profile used to resolve the authentication
    * request: it may be null.
    */
    AuthResult handle_auth(zmqpp::message *msg) noexcept;

    /**
     * A mutex used only internally.
     *
     * It's needed in order to safely replace the mapper_ when
     * reloading the configuration.
     */
    std::mutex mutex_;

    /**
    * Authentication config file parser.
    */
    FileAuthSourceMapperPtr mapper_;

    /**
    * Socket to write to the bus.
    */
    zmqpp::socket bus_push_;

    /**
    * Socket to read from the bus.
    */
    zmqpp::socket bus_sub_;

    /**
    * Name of this auth context instance.
    */
    std::string name_;

    /**
    * Name of the target we auth against.
    */
    std::string target_name_;

    /**
    * Path to the auth data file.
    */
    std::string file_path_;

    CoreUtilsPtr core_utils_;
};
}
}
}
