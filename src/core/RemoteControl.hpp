#pragma once

#include <zmqpp/zmqpp.hpp>
#include <boost/property_tree/ptree_fwd.hpp>

namespace Leosac
{

    class Kernel;

    /**
    * This class handle the remote control of leosac.
    *
    * It's responsibility is to expose a secure socket on the network and filter
    * request from the world, passing legit message to the rest of the application.
    *
    * Configuration option:
    *    + port
    *    + z85 encoded private key
    *
    * Commands:
    * MODULE_LIST
    *
    * response: module_name*
    * module_name: a-Z+
    */
    class RemoteControl
    {
    public:
        RemoteControl(zmqpp::context &ctx, Kernel &kernel, const boost::property_tree::ptree &cfg);

    private:

        void module_list(zmqpp::message *message_out);

        /**
        * Register by core and called when message arrives.
        */
        void handle_msg();

        void process_config(const boost::property_tree::ptree &cfg);

        Kernel &kernel_;

        /**
        * Public ROUTER
        */
        zmqpp::socket socket_;

        // Allow kernel full access to this class.
        friend class Kernel;
    };
}
