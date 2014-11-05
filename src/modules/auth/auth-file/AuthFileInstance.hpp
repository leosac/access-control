#pragma once

#include <zmqpp/zmqpp.hpp>
#include <fstream>

namespace Leosac
{
    namespace Module
    {
        namespace Auth
        {

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
                * @param valid_input_file Path to the file that contains valid input data.
                *
                * @note If the valid_input_file cannot be opened the constructor will throw.
                */
                AuthFileInstance(zmqpp::context &ctx,
                        const std::string &auth_ctx_name,
                        const std::string &auth_target_name,
                        const std::string &valid_input_file);

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
                * File stream to the valid-credentials file.
                */
                std::ifstream file_stream_;
            };

        }
    }
}
