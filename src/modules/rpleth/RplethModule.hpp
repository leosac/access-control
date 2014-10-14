#pragma once

#include "modules/BaseModule.hpp"
#include <memory>
#include <vector>

/**
* This module adds support for Rpleth protocol.
*
* It allows remote control of Wiegand Reader devices.
*/
class RplethModule : public BaseModule
{
public:
    RplethModule(zmqpp::context &ctx,
            zmqpp::socket *pipe,
            const boost::property_tree::ptree &cfg);

    RplethModule(const RplethModule &) = delete;
    RplethModule &operator=(const RplethModule &) = delete;
private:
    //void process_config();
};
