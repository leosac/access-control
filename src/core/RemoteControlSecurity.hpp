#pragma once

#include <boost/property_tree/ptree.hpp>

namespace Leosac
{

    class Kernel;

    /**
    * Provide some kind of security framework to the Remote Control service.
    *
    * This allows relatively fine-grained permissions and allows user
    * to chose who has access to the remote control, and how much permission they got.
    *
    *
    * Remote user are identified using the curve private key they use to connect.
    */
    class RemoteControlSecurity
    {
    public:
        /**
        * The config tree is the same tree that the RemoteControl object has.
        */
        RemoteControlSecurity(const boost::property_tree::ptree &cfg);

        bool allow_request(const std::string &req, const std::string &user_pubkey);

    private:
        boost::property_tree::ptree cfg_;
    };
}

