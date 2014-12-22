#pragma once

#include <zmqpp/zmqpp.hpp>
#include <boost/property_tree/ptree.hpp>
#include <vector>
#include <modules/BaseModule.hpp>
#include "AuthFileInstance.hpp"

namespace Leosac
{
    namespace Module
    {
        namespace Auth
        {

            class AuthFileInstance;

            /**
            * This implements a authentication module that use files to store access permissions.
            *
            * This module conforms to @ref auth_specc.
            * @see @ref mod_auth_file_user_config for end user doc
            */
            class AuthFileModule : public BaseModule
            {
            public:
                AuthFileModule(zmqpp::context &ctx,
                        zmqpp::socket *pipe,
                        const boost::property_tree::ptree &cfg);

                AuthFileModule(const AuthFileModule &) = delete;

                ~AuthFileModule();

            private:

                /**
                * Processing the configuration tree, spawning AuthFileInstance object as described in the
                * configuration file.
                */
                void process_config();

                /**
                * Authenticator instance.
                */
                std::vector<AuthFileInstancePtr> authenticators_;
            };

        }
    }
}
