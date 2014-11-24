#pragma once

#include <zmqpp/zmqpp.hpp>
#include <fstream>

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
                * @param auth_target_name name of the target device we watch
                * @param input_file path to file contain auth configuration
                */
                AuthFileInstance(zmqpp::context &ctx,
                        const std::string &auth_ctx_name,
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

            private:

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
                * Path to the auth data file.
                */
                std::string file_path_;
            };
        }
    }
}
