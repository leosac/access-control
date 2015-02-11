#pragma once

#include <boost/property_tree/ptree.hpp>
#include <unordered_map>

namespace Leosac
{

    class Kernel;

    /**
    * Provide some kind of security framework to the Remote Control service.
    *
    * This allows relatively fine-grained permissions and allows user
    * to chose who has access to the remote control, and how much permission they got.
    *
    * Remote user are identified using the curve private key they use to connect.
    *
    * Implementation Notes:
    *     We store a map of z85-encoded-pubkey <--> list (command name);
    *     The meaning of this map depends on the default permission for user.
    *
    *     If the user is by default granted permission, then the map holds a list command
    *     the user isn't allowed to perform. Otherwise, we store a list of allowed command.
    *
    */
    class RemoteControlSecurity
    {
    public:
        /**
        * The config tree is the same tree that the RemoteControl object has.
        */
        RemoteControlSecurity(const boost::property_tree::ptree &cfg);

        bool allow_request(const std::string &user_pubkey, const std::string &req);

        using KeyCommandsMap = std::unordered_map<std::string, std::vector<std::string>>;

    private:
        void process_config();

        /**
        * Process one `<map>` entry.
        */
        void process_security_entry(const boost::property_tree::ptree &);

        boost::property_tree::ptree cfg_;

        std::unordered_map<std::string, bool> default_permissions_;
        KeyCommandsMap permissions_;

        /**
        * If everyone has access to everything.
        */
        bool unrestricted_;
    };
}

