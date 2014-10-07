#pragma once

#include <zmqpp/zmqpp.hpp>
#include <boost/property_tree/ptree.hpp>
#include <vector>

class AuthFileInstance;

/**
* This implements a authentication module that use files to store access permissions.
*
* Message passing protocol:
*     1. The module SHALL listen to device it wants to (using the application message bus).
*     2. Upon reception of a input from those / this device(s) the module MUST publish a message on the BUS.
*     3. This message MUST have "S_{AUTH_INSTANCE_NAME}" as topic.
*     4. The first data frame MUST be either "GRANTED" or "DENIED".
*     5. Input from device(s) that lead to this authentication handling SHALL be appended to the message.
*
* @note: The AuthFileModule can create multiple AuthFileInstance object.
*/
class AuthFileModule
{
public:
    AuthFileModule(zmqpp::context &ctx,
            zmqpp::socket *pipe,
            const boost::property_tree::ptree &cfg);

    AuthFileModule(const AuthFileModule &) = delete;

    /**
    * Module's main loop. Will exit upon reception of signal::stop from module manager
    */
    void run();

    /**
    * Watch from stop signal from module manager
    */
    void handle_pipe();

private:

    /**
    * Processing the configuration tree, spawning AuthFileInstance object as described in the
    * configuration file.
    */
    void process_config();

    zmqpp::context &ctx_;

    /**
    * Pipe back to module manager
    */
    zmqpp::socket &pipe_;

    zmqpp::reactor reactor_;

    boost::property_tree::ptree config_;

    bool is_running_;

    /**
    * Authenticator instance.
    */
    std::vector<AuthFileInstance *> authenticators_;
};
