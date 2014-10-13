#pragma once

#include <zmqpp/zmqpp.hpp>
#include "FakeGPIO.hpp"
#include <boost/property_tree/ptree.hpp>
#include "gtest/gtest.h"
#include "core/MessageBus.hpp"

/**
* Helper function that create an object of type ModuleType (using conventional parameter) and run it.
*/
template<typename ModuleType>
bool test_run_module(zmqpp::context *ctx, zmqpp::socket *pipe, const boost::property_tree::ptree &cfg)
{
    ModuleType module(*ctx, pipe, cfg);

    pipe->send(zmqpp::signal::ok);
    module.run();
    return true;
}
