#pragma once

#include "zmqpp/zmqpp.hpp"
#include "WiegandReaderImpl.hpp"

/**
* Module providing support for Wiegand Reader.
* This module allows user to register multiple wiegand reader in the configuration file.
*
* Message passing protocol:
*     1. Upon reception of what looks like a full wiegand frame, a message MUST be published to the application bus.
*     2. This message SHALL contain the card-id that was read, in hexadecimal format, for example:
*/
class WiegandReaderModule
{
public:
    WiegandReaderModule(zmqpp::context &ctx,
            zmqpp::socket *pipe,
            const boost::property_tree::ptree &cfg);

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
    * Create wiegand reader instance based on configuration.
    */
    void process_config();

    zmqpp::reactor reactor_;

    /**
    * Vector of wiegand reader managed by this module.
    */
    std::vector<WiegandReaderImpl> readers_;

    zmqpp::context &ctx_;
    zmqpp::socket &pipe_;
    boost::property_tree::ptree config_;

    bool is_running_;
};
