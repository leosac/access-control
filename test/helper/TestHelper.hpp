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

/**
* Part of the `bus_read()` stuff.
*/
bool bus_read_extract(zmqpp::message *m)
{
    return true;
}

/**
* Default frame extraction function.
*/
template<typename T, typename ...Content>
bool bus_read_extract(zmqpp::message *m, T first_arg, Content... content)
{
    T value;
    *m >> value;

    if (value != first_arg)
        return false;
    return bus_read_extract(m, content...);
}

/**
* Frame extraction method specialized (thanks to overloading) for `const char *`
*/
template<typename ...Content>
bool bus_read_extract(zmqpp::message *m, const char * first_arg, Content... content)
{
    std::string value;
    *m >> value;

    if (strcmp(value.c_str(), first_arg) != 0)
        return false;
    return bus_read_extract(m, content...);
}

/**
* Make a blocking read on the bus, return true if content match the message.
* false otherwise.
*/
template<typename ...Content>
bool bus_read(zmqpp::socket &sub, Content... content)
{
    zmqpp::message msg;

    if (!sub.receive(msg))
        return false;
    return bus_read_extract(&msg, content...);
}
