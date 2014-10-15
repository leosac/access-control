#pragma once

#include <zmqpp/zmqpp.hpp>
#include <boost/property_tree/ptree.hpp>
#include <vector>

class DoormanInstance;

/**
* The doorman module allows Leosac's users to configure the behavior to follow
* when authentication event happens.
* The doorman module can spawn multiple doorman instance, each taking
* care of their own stuff.
*
* Message passing protocol:
*    1. Not defined yet.
*/
class DoormanModule
{
public:
    DoormanModule(zmqpp::context &ctx,
            zmqpp::socket *pipe,
            const boost::property_tree::ptree &cfg);

    DoormanModule(const DoormanModule &) = delete;
    DoormanModule &operator=(const DoormanModule &) = delete;

    ~DoormanModule();

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
    std::vector<DoormanInstance *> doormen_;
};