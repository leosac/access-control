#pragma once

#include "zmqpp/zmqpp.hpp"

/**
* Module providing support for Wiegand Reader.
*/
class WiegandReaderModule
{
public:
    WiegandReaderModule(zmqpp::context &ctx,
            zmqpp::socket *pipe,
            const boost::property_tree::ptree &cfg);

private:

    /**
    * Create wiegand reader instance based on configuration.
    */
    void process_config();

    zmqpp::context &ctx_;
    zmqpp::socket &pipe_;
    boost::property_tree::ptree config_;
};
