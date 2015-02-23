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

#include <zmqpp/zmqpp.hpp>
#include <fstream>
#include "FileAuthSourceMapper.hpp"

namespace Leosac
{
    namespace Module
    {
        namespace Auth
        {

            class AuthFileInstance;
            using AuthFileInstancePtr = std::shared_ptr<AuthFileInstance>;

            /**
            * An instance of an authentication handler that use files to check whether or not access is granted or denied.
            * This class is for INTERNAL use only (by AuthFileModule).
            */
            class AuthFileInstance
            {
            public:
                /**
                * Create a new Authenticator that watch a device and emit authentication message.
                * @param ctx the ZeroMQ context
                * @param auth_ctx_name name of this authentication context.
                * @param auth_sources_names names of the sources devices we watch (ie wiegand reader).
                * @param auth_target_name name of the target (ie door) we auth against.
                * @param input_file path to file contain auth configuration
                */
                AuthFileInstance(zmqpp::context &ctx,
                        const std::string &auth_ctx_name,
                        const std::list<std::string> &auth_sources_names,
                        const std::string &auth_target_name,
                        const std::string &input_file);

                ~AuthFileInstance();

                AuthFileInstance(const AuthFileInstance &) = delete;

                AuthFileInstance &operator=(const AuthFileInstance &) = delete;

                /**
                * Something happened on the bus that we have interest into.
                */
                void handle_bus_msg();

                /**
                * Prepare auth source object, map them to profile and check if access is granted.
                *
                * @note This is a `noexcept` method. Will return false in case something went wrong.
                * @return true is access shall be granted, false otherwise.
                */
                bool handle_auth(zmqpp::message *msg) noexcept;

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
                * Authentication config file parser.
                */
                FileAuthSourceMapper mapper_;

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
            };
        }
    }
}
