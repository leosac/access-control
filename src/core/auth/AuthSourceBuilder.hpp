#include <core/auth/Interfaces/IAuthenticationSource.hpp>
#include <zmqpp/message.hpp>

#pragma once

namespace Leosac
{
    namespace Auth
    {
        /**
        * This class is some kind of factory to create IAuthenticationSource object
        * from a zmqpp::message sent by a AuthSource module (eg Wiegand).
        *
        * The AuthenticationSource will not contain all data. It still needs to be mapped
        * to a user/access profile.
        */
        class AuthSourceBuilder
        {
        public:
            explicit AuthSourceBuilder() = default;
            virtual ~AuthSourceBuilder() = default;
            AuthSourceBuilder(const AuthSourceBuilder &) = delete;
            AuthSourceBuilder(AuthSourceBuilder &&) = delete;
            AuthSourceBuilder &operator=(const AuthSourceBuilder &) = delete;
            AuthSourceBuilder &operator=(AuthSourceBuilder &&) = delete;

            /**
            * Create an AuthenticationSource object from a message.
            *
            * @param msg message from auth source module, CANNOT BE NULL.
            */
            virtual IAuthenticationSourcePtr create(zmqpp::message *msg);

            /**
            * Extract the source name from the frame.
            * @param output shall not be null.
            * @return true if it succeed, false otherwise.
            * @see @ref auth_specc
            */
            bool extract_source_name(const std::string &input, std::string *output) const;

        protected:
            /**
            * Create an auth source from SIMPLE_WIEGAND data type.
            * @param msg the message first frame shall be wiegand data (ie previous should be pop'd).
            */
            IAuthenticationSourcePtr create_simple_wiegand(const std::string &name,
                    zmqpp::message *msg);
        };
    }
}
