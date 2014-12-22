#pragma once

#include <zmqpp/zmqpp.hpp>
#include <boost/property_tree/ptree.hpp>
#include <vector>
#include <modules/BaseModule.hpp>

namespace Leosac
{
    namespace Module
    {
        /**
        * Module that allows user to configure action to be taken
        * to react to messages from other modules.
        *
        * @see @ref mod_doorman_main for end-user documentation.
        */
        namespace Doorman
        {

            class DoormanInstance;

            /**
            * Main class for the module, it create handlers and run them
            * to, well, handle events and send command.
            *
            * @see @ref mod_doorman_user_config for configuration information.
            */
            class DoormanModule : public BaseModule
            {
            public:
                DoormanModule(zmqpp::context &ctx,
                        zmqpp::socket *pipe,
                        const boost::property_tree::ptree &cfg);

                DoormanModule(const DoormanModule &) = delete;

                DoormanModule &operator=(const DoormanModule &) = delete;

                ~DoormanModule();

            private:

                /**
                * Processing the configuration tree, spawning AuthFileInstance object as described in the
                * configuration file.
                */
                void process_config();

                /**
                * Authenticator instance.
                */
                std::vector<DoormanInstance *> doormen_;
            };

        }
    }
}